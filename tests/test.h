#ifndef TEST_H
# define TEST_H

# include <libmpm.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <errno.h>

# define DUP_ALL_OUTPUTS(name) dup2(name[1], 1); dup2(name[1], 2); close(name[1]);
# define WAIT_AND_CLOSE(pid, status, fd) pid = waitpid(pid, &status, 0); close(fd[1]);


void		register_flags_test(void);
void		register_test_database(void);
void		register_test_files(void);
void		register_test_category(void);
void		register_test_config(void);
void        register_test_packer(void);
void        register_test_compile(void);
void        register_test_error(void);

void		clean_db(const char *name);
int			free_sql_results(void *ptr);
int			free_single_result_sql(void *ptr);

typedef struct		sql_result_s {
	char		*name;
	char		*value;
}					sql_result_t;

#endif /* TEST_H */
