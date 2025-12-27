#define _GNU_SOURCE
#include <mr_utils.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define INIT_ARGS_CAPACITY 64
#define INIT_FILES_CAPACITY 256
#define VERSION "0.1.1"

typedef enum Option {
	OPTION_HELP_SHORT,
	OPTION_HELP,
	OPTION_VERSION_SHORT,
	OPTION_VERSION,
	OPTION_CHECK_SHORT,
	OPTION_CHECK,
	OPTION_COUNT,
} Option;

const char *option_to_str[OPTION_COUNT] = { "-h",	 "--help", "-v",
					    "--version", "-c",	   "--check" };
Bool is_check = FALSE;

void log_help(MrlLogger *logger)
{
	mrl_logln(logger, MRL_SEVERITY_DEFAULT,
		  "tool to remove trailing whitespace");
	mrl_logln(logger, MRL_SEVERITY_DEFAULT,
		  "Usage: spacers [OPTION] [FILE]... ");
	mrl_logln(logger, MRL_SEVERITY_DEFAULT, "");
	mrl_logln(logger, MRL_SEVERITY_DEFAULT, "Options:");
	mrl_logln(
		logger, MRL_SEVERITY_DEFAULT,
		"  -c, --check        Checks for whitespace. does not mutate files");
	mrl_logln(logger, MRL_SEVERITY_DEFAULT,
		  "  -h, --help         Print help ");
	mrl_logln(logger, MRL_SEVERITY_DEFAULT,
		  "  -v, --version      Print version");
}

void log_version(MrlLogger *logger)
{
	mrl_log(logger, MRL_SEVERITY_DEFAULT, "spacers ");
	mrl_logln(logger, MRL_SEVERITY_DEFAULT, VERSION);
}

void log_invalid_arg(MrlLogger *logger, MrsString *arg)
{
	mrl_log(logger, MRL_SEVERITY_ERROR, "error: invalid argument ");
	mrl_log(logger, MRL_SEVERITY_ERROR, "\"");
	mrl_log(logger, MRL_SEVERITY_ERROR, arg->value);
	mrl_logln(logger, MRL_SEVERITY_ERROR, "\"");
}

Err process_arg(MrlLogger *logger, MrsString *arg)
{
	Option option = OPTION_COUNT;
	for (int i = 0; i < OPTION_COUNT; i++) {
		if (strcmp(arg->value, option_to_str[i]) == 0) {
			option = i;
		}
	}

	switch (option) {
	case OPTION_HELP_SHORT:
	case OPTION_HELP:
		log_help(logger);
		break;
	case OPTION_VERSION_SHORT:
	case OPTION_VERSION:
		log_version(logger);
		break;
	case OPTION_CHECK_SHORT:
	case OPTION_CHECK:
		is_check = TRUE;
		break;
	case OPTION_COUNT:
		log_invalid_arg(logger, arg);
		return ERR;
	default:
		break;
	}
	return OK;
}

Err write_line_to_file(FILE *file, MrsString *line)
{
	mrs_pushstr(line, "\n", 1);
	size_t written_len = fwrite(line->value, sizeof(char), line->len, file);
	if (written_len != line->len) {
		return ERR;
	}

	return OK;
}

Err check_file(MrlLogger *logger, FILE *file)
{
	Bool last_line_is_empty = FALSE;
	uint line_number = 0;
	for (;;) {
		char *lineptr = NULL;
		size_t space_count = CAFE_BABE;

		ssize_t len = getline(&lineptr, &space_count, file);
		if (len == -1) {
			free(lineptr);
			break;
		}

		line_number++;

		if (len > 1 && lineptr[len - 2] == ' ') {
			lineptr[len - 1] = '\0';
			mrl_logln(logger, MRL_SEVERITY_ERROR,
				  "trailing whitespace found on line %zu: '%s'",
				  line_number, lineptr);
			free(lineptr);
			return ERR;
		}

		if (len == 1 && lineptr[0] == '\n') {
			last_line_is_empty = TRUE;
		} else {
			last_line_is_empty = FALSE;
		}

		free(lineptr);
	}

	if (last_line_is_empty) {
		mrl_logln(logger, MRL_SEVERITY_ERROR,
			  "extra newlines found at end of file");
		return ERR;
	}

	return OK;
}

Err write_file(MrlLogger *logger, FILE *file)
{
	MrvVector lines;
	mrv_init(&lines, 1024, sizeof(MrsString));

	for (;;) {
		char *lineptr = NULL;
		size_t space_count = 0;
		ssize_t len = getline(&lineptr, &space_count, file);

		if (len == -1) {
			free(lineptr);
			break;
		}

		MrsString trimed_line;
		mrs_init(len, lineptr, len, &trimed_line);

		mrs_trim_trailing_whitespace(&trimed_line);

		mrv_append(&lines, &trimed_line);
		free(lineptr);
	}

	while (lines.len > 0) {
		MrsString *last_line = mrv_get_last(&lines);

		if (last_line->len == 0) {
			mrs_free(last_line);
			lines.len--;
		} else {
			// empty line
			break;
		}
	}

	rewind(file);

	for (size_t i = 0; i < lines.len; i++) {
		MrsString *line = mrv_get_idx(&lines, i);
		Err err = write_line_to_file(file, line);
		if (err != OK) {
			mrl_logln(logger, MRL_SEVERITY_ERROR,
				  "couldnt write to file");

			for (size_t j = 0; j < lines.len; j++)
				mrs_free(mrv_get_idx(&lines, j));
			mrv_free(&lines);

			return ERR;
		}
	}

	fflush(file);
	int fd = fileno(file);
	off_t current_pos = ftell(file);

	if (ftruncate(fd, current_pos) != 0) {
		mrl_logln(logger, MRL_SEVERITY_ERROR,
			  "could not truncate file");
		return ERR;
	}

	for (size_t i = 0; i < lines.len; i++) {
		mrs_free(mrv_get_idx(&lines, i));
	}

	mrv_free(&lines);

	return OK;
}

Err process_files(MrlLogger *logger, MrvVector *file_paths)
{
	for (size_t i = 0; i < file_paths->len; i++) {
		MrsString *file_path = mrv_get_idx(file_paths, i);
		FILE *file = fopen(file_path->value, "r+");
		if (file == NULL) {
			mrl_log(logger, MRL_SEVERITY_ERROR,
				"File could not be opened -> ");
			mrl_logln(logger, MRL_SEVERITY_ERROR, file_path->value);
			continue;
		}
		if (is_check == FALSE) {
			Err err = write_file(logger, file);
			if (err) {
				mrs_free(file_path);
				fclose(file);
				exit(1);
			} else {
				mrl_logln(logger, MRL_SEVERITY_DEFAULT,
					  file_path->value);
			}
		} else {
			Err err = check_file(logger, file);
			if (err) {
				mrl_log(logger, MRL_SEVERITY_DEFAULT,
					file_path->value);
				mrl_logln(logger, MRL_SEVERITY_ERROR,
					  " whitespace found");
				mrs_free(file_path);
				fclose(file);
				exit(1);
			} else {
				mrl_log(logger, MRL_SEVERITY_DEFAULT,
					file_path->value);
				mrl_logln(logger, MRL_SEVERITY_OK, " OK");
			}
		}
		fclose(file);
	}
	return OK;
}

int main(int argc, char **argv)
{
	MrlLogger *logger = mrl_create(stderr, TRUE, FALSE);
	MrvVector args;
	mrv_init(&args, INIT_ARGS_CAPACITY, sizeof(MrsString));
	for (size_t i = 0; i < (size_t)argc; i++) {
		MrsString arg;
		mrs_init(strlen(argv[i]), argv[i], strlen(argv[i]), &arg);
		mrs_remove_whitespace(&arg);
		mrv_append(&args, &arg);
	}
	if (argc == 1) {
		log_help(logger);
	}
	MrvVector file_paths;
	mrv_init(&file_paths, INIT_FILES_CAPACITY, sizeof(MrsString));
	Err err = OK;
	for (size_t i = 1; i < args.len; i++) {
		MrsString *arg = mrv_get_idx(&args, i);
		switch (arg->value[0]) {
		case '-':
			err = process_arg(logger, arg);
			break;
		default:
			(void)0; // dafak
			MrsString path;
			mrs_init(strlen(argv[i]), argv[i], strlen(argv[i]),
				 &path);
			mrv_append(&file_paths, &path);
			break;
		}
		if (err == ERR) {
			break;
		}
	}
	if (err == OK) {
		err = process_files(logger, &file_paths);
	}
	for (size_t i = 0; i < file_paths.len; i++) {
		mrs_free(mrv_get_idx(&file_paths, i));
	}
	mrv_free(&file_paths);
	for (size_t i = 0; i < args.len; i++) {
		mrs_free(mrv_get_idx(&args, i));
	}
	mrv_free(&args);
	mrl_destroy(logger);
}
