#include "../mrd_debug.h"
#include "../mrl_logger.h"
#include "../mrs_strings.h"
#include "../mrv_vectors.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_ARGS 64
#define MAX_FILES 256
#define MAX_ARG_LENGTH
#define VERSION "0.1.0"

typedef enum Option {
	OPTION_HELP_SHORT,
	OPTION_HELP,

	OPTION_VERSION_SHORT,
	OPTION_VERSION,

	OPTION_CHECK_SHORT,
	OPTION_CHECK,

	OPTION_COUNT,
} Option;

const char *optionToStr[OPTION_COUNT] = { "-h",	       "--help", "-v",
					  "--version", "-c",	 "--check" };

Bool is_check = FALSE;

void log_help(MrlLogger *logger)
{
	mrl_logln(logger, "tool to remove trailing whitespace",
		  MRL_SEVERITY_DEFAULT);
	mrl_logln(logger, "Usage: spacers [OPTION] [FILE]... ",
		  MRL_SEVERITY_DEFAULT);
	mrl_logln(logger, "", MRL_SEVERITY_DEFAULT);
	mrl_logln(logger, "Options:", MRL_SEVERITY_DEFAULT);
	mrl_logln(
		logger,
		"  -c, --check        Checks for whitespace. does not mutate files",
		MRL_SEVERITY_DEFAULT);
	mrl_logln(logger, "  -h, --help         Print help ",
		  MRL_SEVERITY_DEFAULT);
	mrl_logln(logger, "  -v, --version      Print version",
		  MRL_SEVERITY_DEFAULT);
}

void log_version(MrlLogger *logger)
{
	mrl_log(logger, "spacers ", MRL_SEVERITY_DEFAULT);
	mrl_logln(logger, VERSION, MRL_SEVERITY_DEFAULT);
}

void log_invalid_arg(MrlLogger *logger, MrsString *arg)
{
	mrl_log(logger, "error: invalid argument ", MRL_SEVERITY_ERROR);
	mrl_log(logger, "\"", MRL_SEVERITY_ERROR);
	mrl_log(logger, arg->value, MRL_SEVERITY_ERROR);
	mrl_logln(logger, "\"", MRL_SEVERITY_ERROR);
}

Err process_arg(MrlLogger *logger, MrsString *arg)
{
	Option option = OPTION_COUNT;

	for (int i = 0; i < OPTION_COUNT; i++) {
		if (strcmp(arg->value, optionToStr[i]) == 0) {
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

size_t get_trimed_line_length(char *str, int len)
{
	MrsString string;
	mrs_init(len, str, len, &string);
	mrs_trim_trailing_whitespace(&string);
	size_t trimed_len = string.len;

	mrs_free(&string);

	return trimed_len;
}

internal Err write_to_temp(FILE *temp_file, MrsString *line)
{
	size_t written_len =
		fwrite(line->value, sizeof(char), line->len, temp_file);
	if (written_len != line->len) {
		return ERR;
	}

	fwrite("\n", sizeof(char), 1, temp_file);

	return OK;
}

internal Err check_file(FILE *file)
{
	Bool end_of_file = FALSE;

	while (end_of_file == FALSE) {
		char *lineptr = NULL;
		size_t space_count = CAFE_BABE;

		long len = getline(&lineptr, &space_count, file);

		if (len == EOF) {
			end_of_file = TRUE;
		}
		if (len == -1) {
			free(lineptr);
			break;
		}

		MrsString trimed_line;
		mrs_init(len, lineptr, len, &trimed_line);
		mrs_trim_trailing_whitespace(&trimed_line);

		// +1 because no '\n'
		if (trimed_line.len + 1 != (size_t)len) {
			mrs_free(&trimed_line);
			free(lineptr);
			return ERR;
		}

		mrs_free(&trimed_line);

		free(lineptr);
	}

	return OK;
}

internal Err write_file(MrlLogger *logger, FILE *file)
{
	FILE *tmp_file = tmpfile();

	for (;;) {
		char *lineptr = NULL;
		size_t space_count = CAFE_BABE;

		long len = getline(&lineptr, &space_count, file);

		if (len == EOF) {
			free(lineptr);
			break;
		}
		if (len == -1) {
			free(lineptr);
			break;
		}

		MrsString trimed_line;
		mrs_init(len, lineptr, len, &trimed_line);
		mrs_trim_trailing_whitespace(&trimed_line);

		Err err = write_to_temp(tmp_file, &trimed_line);
		if (err != OK) {
			mrl_logln(logger,
				  "couldnt write all elements to a line",
				  MRL_SEVERITY_ERROR);
			return ERR;
		}

		mrs_free(&trimed_line);

		free(lineptr);
	}

	char a;
	fseek(tmp_file, 0, SEEK_SET);
	fseek(file, 0, SEEK_SET);
	while ((a = fgetc(tmp_file)) != EOF) {
		fputc(a, file);
	}

	int trunc_err  = ftruncate(fileno(file), ftell(tmp_file));

        fclose(tmp_file);

        if(trunc_err != 0) {
                return ERR;
        }

	return OK;
}

internal Err process_files(MrlLogger *logger, MrvVector *file_paths)
{
	for (size_t i = 0; i < file_paths->len; i++) {
		MrsString *file_path = mrv_get_idx(file_paths, i);

		FILE *file = fopen(file_path->value, "r+");
		if (file == NULL) {
			mrl_log(logger, "File could not be opened -> ",
				MRL_SEVERITY_ERROR);
			mrl_logln(logger, file_path->value, MRL_SEVERITY_ERROR);
			continue;
		}

		if (is_check == FALSE) {
			Err err = write_file(logger, file);
			if (err != OK) {
				mrs_free(file_path);
				fclose(file);
				exit(1);
			} else {
				mrl_logln(logger, file_path->value,
					  MRL_SEVERITY_DEFAULT);
			}
		} else {
			Err err = check_file(file);
			if (err != OK) {
				mrl_log(logger, file_path->value,
					MRL_SEVERITY_DEFAULT);
				mrl_logln(logger, " whitespace found",
					  MRL_SEVERITY_ERROR);
				mrs_free(file_path);
				fclose(file);
				exit(1);
			} else {
				mrl_log(logger, file_path->value,
					MRL_SEVERITY_DEFAULT);
				mrl_logln(logger, " OK", MRL_SEVERITY_OK);
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
	mrv_init(&args, MAX_ARGS, sizeof(MrsString));

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
	mrv_init(&file_paths, MAX_FILES, sizeof(MrsString));

	Err err = OK;
	for (size_t i = 1; i < args.len; i++) {
		MrsString *arg = mrv_get_idx(&args, i);

		switch (arg->value[0]) {
		case '-':
			err = process_arg(logger, arg);
			break;
		default:
			(void)err; // dafak
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
