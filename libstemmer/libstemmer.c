
#include "libstemmer.h"
#include "../runtime/api.h"
#include "modules.h"

struct sb_stemmer {
    struct SN_env * (*create)(void);
    void (*close)(struct SN_env *);
    int (*stem)(struct SN_env *);

    struct SN_env * env;
};

const char **
sb_stemmer_list()
{
    return algorithm_names;
}

struct sb_stemmer *
sb_stemmer_new(const char * algorithm)
{
    struct stemmer_modules * module;
    struct sb_stemmer * stemmer =
	    (struct sb_stemmer *) malloc(sizeof(struct sb_stemmer));
    if (stemmer == 0) return 0;

    for (module = modules; module->name != 0; module++) {
	if (strcmp(module->name, algorithm) == 0) break;
    }
    if (module->name == 0) return 0;
    
    stemmer->create = module->create;
    stemmer->close = module->close;
    stemmer->stem = module->stem;

    stemmer->env = stemmer->create();

    return stemmer;
}

void
sb_stemmer_delete(struct sb_stemmer * stemmer)
{
    if (stemmer == 0) return;
    if (stemmer->close == 0) return;
    stemmer->close(stemmer->env);
    stemmer->close = 0;
    free(stemmer);
}

const sb_symbol *
sb_stemmer_stem(struct sb_stemmer * stemmer, const sb_symbol * word, int size)
{
    SN_set_current(stemmer->env, size, word);
    (void) stemmer->stem(stemmer->env);
    stemmer->env->p[stemmer->env->l] = 0;
    return stemmer->env->p;
}

int
sb_stemmer_length(struct sb_stemmer * stemmer)
{
    return stemmer->env->l;
}
