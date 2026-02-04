#include "internals.h"
#include <mr_utils.h>

#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <unistd.h>

#define MRT_INIT_TEST_CASES_PER_GROUP 64
#define MRT_INIT_TEST_GROUPS_PER_CONTEXT 64
#define MRT_TAB "    "

typedef struct {
	MrsString description;
	Bool pass;
	MrsString predicate;
	MrsString file_name;
	int line_number;
} MrtCase;

mr_internal void mrt_group_init(struct MrtGroup *t_group,
				const char *description, MrtTestFunc func);

mr_internal void mrt_group_destroy(struct MrtGroup *t_group);

mr_internal Err mrt_group_log(struct MrtGroup *t_group,
			      struct MrlLogger *logger);

mr_internal void mrt_group_log_failure(MrlLogger *logger,
				       struct MrtGroup *group);

mr_internal void mrt_case_log(struct MrlLogger *mrl_ctx, MrtCase test_case);

struct MrtContext *mrt_ctx_create(MrlLogger *logger)
{
	struct MrtContext *t_ctx = malloc(sizeof(*t_ctx));
	memset(t_ctx, 0, sizeof(*t_ctx));

	mrv_init(&t_ctx->test_groups, MRT_INIT_TEST_GROUPS_PER_CONTEXT,
		 sizeof(struct MrtGroup));

	t_ctx->logger = logger;

	return t_ctx;
}

void mrt_ctx_destroy(struct MrtContext *ctx)
{
	for (uint i = 0; i < ctx->test_groups.len; i++) {
		struct MrtGroup *t_group = mrv_get_idx(&ctx->test_groups, i);
		mrt_group_destroy(t_group);
	}

	mrv_free(&ctx->test_groups);

	free(ctx);
}

void mrt_ctx_register_test_func(struct MrtContext *ctx, MrtTestFunc t_func,
				const char *t_func_name)
{
	struct MrtGroup t_group;
	mrt_group_init(&t_group, t_func_name, t_func);

	mrv_append(&ctx->test_groups, &t_group, APPEND_SCALING_ONE_POINT_FIVE);

	return;
}

mr_global mtx_t *logging_mutex = NULL;
mr_internal int run_testgroup_with_locking_logging(struct MrtContext *ctx,
						   int t_group_idx)
{
	struct MrtGroup *group = mrv_get_idx(&ctx->test_groups, t_group_idx);

	(*group->func)(group);

	mtx_lock(logging_mutex);
	Err err = mrt_group_log(group, ctx->logger);
	mtx_unlock(logging_mutex);

	return err;
}

mr_global mtx_t *atomic_add_group_idx_mutex = NULL;
mr_internal size_t atomic_fetch_add_group_idx(size_t *x, size_t increment)
{
	mtx_lock(atomic_add_group_idx_mutex);
	*x += increment;
	mtx_unlock(atomic_add_group_idx_mutex);
	return *x - increment;
}

struct WorkerParams {
	struct MrtContext *ctx;
	size_t *next_group_idx; // is atomic. BEWARE
	size_t total_groups;
};
mr_internal int worker_thread_func(void *worker_contex)
{
	struct WorkerParams *w_ctx = worker_contex;
	int total_worker_err = 0;

	for (;;) {
		size_t group_idx_to_run =
			atomic_fetch_add_group_idx(w_ctx->next_group_idx, 1);

		if (group_idx_to_run >= w_ctx->total_groups)
			break;

		total_worker_err += run_testgroup_with_locking_logging(
			w_ctx->ctx, group_idx_to_run);
	}

	return total_worker_err;
}

mr_internal int mrt_ctx_run_parrallelized(struct MrtContext *ctx)
{
	int core_count = sysconf(_SC_NPROCESSORS_ONLN);
	if (core_count < 1) {
		core_count = 1;
	}

	size_t thread_count = core_count;
	if (thread_count > ctx->test_groups.len) {
		thread_count = ctx->test_groups.len;
	}

	size_t next_group_idx = 0;
	struct WorkerParams w_ctx;
	w_ctx.ctx = ctx;
	w_ctx.next_group_idx = &next_group_idx;
	w_ctx.total_groups = ctx->test_groups.len;

	logging_mutex = malloc(sizeof(*logging_mutex));
	mtx_init(logging_mutex, mtx_plain);

	atomic_add_group_idx_mutex =
		malloc(sizeof(*atomic_add_group_idx_mutex));
	mtx_init(atomic_add_group_idx_mutex, mtx_plain);

	thrd_t *threads = calloc(thread_count, sizeof(thrd_t));

	for (uint i = 0; i < thread_count; i++) {
		thrd_create(&threads[i], worker_thread_func, &w_ctx);
	}

	size_t err_count = 0;
	for (uint i = 0; i < thread_count; i++) {
		int err;
		thrd_join(threads[i], &err);
		err_count += err;
	}

	free(threads);

	mtx_destroy(atomic_add_group_idx_mutex);
	free(atomic_add_group_idx_mutex);

	mtx_destroy(logging_mutex);
	free(logging_mutex);

	return err_count;
}

mr_internal int mrt_ctx_run_single_threaded(struct MrtContext *ctx)
{
	size_t err_count = 0;

	for (uint i = 0; i < ctx->test_groups.len; i++) {
		struct MrtGroup *t_group = mrv_get_idx(&ctx->test_groups, i);

		(*t_group->func)(t_group);

		err_count += mrt_group_log(t_group, ctx->logger);
	}

	return err_count;
}

int mrt_ctx_run(struct MrtContext *ctx, Bool run_tests_parrallelized)
{
	size_t err_count = 0;

	if (run_tests_parrallelized) {
		err_count = mrt_ctx_run_parrallelized(ctx);
	} else {
		err_count = mrt_ctx_run_single_threaded(ctx);
	}

	if (err_count) {
		mrl_logln(ctx->logger, MRL_SEVERITY_DEFAULT, "\nfailures:");
		for (size_t i = 0; i < ctx->test_groups.len; i++) {
			struct MrtGroup *group =
				mrv_get_idx(&ctx->test_groups, i);

			if (group->pass_count != group->cases.len) {
				mrt_group_log_failure(ctx->logger, group);
			}
		}
	}

	mrl_logln(ctx->logger, MRL_SEVERITY_DEFAULT, "");
	mrl_logln(ctx->logger, MRL_SEVERITY_DEFAULT,
		  "test results: %s. %d passed; %d failed",
		  err_count == 0 ? "PASSED" : "FAILED",
		  ctx->test_groups.len - err_count, err_count);

	return err_count;
}

mr_internal void mrt_group_init(struct MrtGroup *t_group,
				const char *description, MrtTestFunc func)
{
	memset(t_group, 0, sizeof(*t_group));

	MrsString s;
	mrs_init(strlen(description), description, strlen(description), &s);
	t_group->name = s;
	t_group->func = func;

	mrv_init(&t_group->cases, MRT_INIT_TEST_CASES_PER_GROUP,
		 sizeof(MrtCase));
}

mr_internal void mrt_group_destroy(struct MrtGroup *t_group)
{
	for (size_t i = 0; i < t_group->cases.len; i++) {
		MrtCase *c = mrv_get_idx(&t_group->cases, i);
		mrs_free(&c->description);
		mrs_free(&c->predicate);
		mrs_free(&c->file_name);
	}

	mrv_free(&t_group->cases);
	mrs_free(&t_group->name);
}

void mrt_group_append_case(struct MrtGroup *t_group, const char *description,
			   Bool pass, const char *predicate,
			   const char *file_name, int line_number)
{
	if (pass) {
		t_group->pass_count++;
	}

	MrsString d;
	mrs_init(strlen(description), description, strlen(description), &d);

	MrsString p;
	mrs_init(strlen(predicate), predicate, strlen(predicate), &p);

	MrsString f;
	mrs_init(strlen(file_name), file_name, strlen(file_name), &f);

	mrv_append(&t_group->cases,
		   &(MrtCase){ .description = d,
			       .pass = pass,
			       .predicate = p,
			       .file_name = f,
			       .line_number = line_number },
		   APPEND_SCALING_ONE_POINT_FIVE);
}

mr_internal Err mrt_group_log(struct MrtGroup *t_group,
			      struct MrlLogger *logger)
{
	mrl_logln(logger, MRL_SEVERITY_DEFAULT, "");

	mrl_log(logger, MRL_SEVERITY_DEFAULT, "test group: ");
	mrl_logln(logger, MRL_SEVERITY_INFO, t_group->name.value);

	for (size_t i = 0; i < t_group->cases.len; i++) {
		mrl_log(logger, MRL_SEVERITY_DEFAULT, MRT_TAB);
		MrtCase *c = mrv_get_idx(&t_group->cases, i);
		mrt_case_log(logger, *c);
	}
	mrl_logln(logger, MRL_SEVERITY_DEFAULT, "");

	char pass_rate[15];
	sprintf(pass_rate, "%zu/%d Passed", t_group->pass_count,
		(int)t_group->cases.len);
	mrl_log(logger, MRL_SEVERITY_DEFAULT, MRT_TAB);
	mrl_logln(logger, MRL_SEVERITY_DEFAULT, pass_rate);

	mrl_log(logger, MRL_SEVERITY_DEFAULT, MRT_TAB);
	if (t_group->pass_count != t_group->cases.len) {
		mrl_logln(logger, MRL_SEVERITY_ERROR, "FAILED");
		return ERR;
	} else {
		mrl_logln(logger, MRL_SEVERITY_OK, "PASSED");
		return OK;
	}
}

mr_internal void mrt_group_log_failure(MrlLogger *logger,
				       struct MrtGroup *group)
{
	mrl_logln(logger, MRL_SEVERITY_DEFAULT, "\n---- %s ----",
		  group->name.value);

	for (size_t j = 0; j < group->cases.len; j++) {
		MrtCase *c = mrv_get_idx(&group->cases, j);

		if (!c->pass) {
			mrl_log(logger, MRL_SEVERITY_DEFAULT, MRT_TAB);
			mrl_logln(logger, MRL_SEVERITY_DEFAULT, "in: %s:%d",
				  c->file_name.value, c->line_number);
			mrl_log(logger, MRL_SEVERITY_DEFAULT, MRT_TAB);
			mrl_logln(logger, MRL_SEVERITY_DEFAULT, "desc: %s",
				  c->description.value);
			mrl_log(logger, MRL_SEVERITY_DEFAULT, MRT_TAB);
			mrl_logln(logger, MRL_SEVERITY_DEFAULT,
				  "assertion: %s\n", c->predicate);
		}
	}
}

mr_internal void mrt_case_log(struct MrlLogger *mrl_ctx, MrtCase test_case)
{
	mrl_log(mrl_ctx, MRL_SEVERITY_DEFAULT, test_case.description.value);
	if (test_case.pass) {
		mrl_logln(mrl_ctx, MRL_SEVERITY_OK, " ... ok");
	} else {
		mrl_logln(mrl_ctx, MRL_SEVERITY_ERROR, " ... FAILED");
	}
	mrl_reset_severity(mrl_ctx);
}
