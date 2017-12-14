#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>

#include <gmp.h>
#include <libgen.h>

void gcd_free(mpz_t *p)   { mpz_clear(*p); free(p); }
void lcm_free(mpz_t *p)   { mpz_clear(*p); free(p); }
void apply_free(mpz_t *p) { mpz_clear(*p); free(p); }

// gcd using euclidean algorithm
// call gcd_free() on result
mpz_t * gcd(const mpz_t x, const mpz_t y) {

	mpz_t *a, *b;

	if (!mpz_sgn(x) || !mpz_sgn(y))
		return NULL;

	if (!(a = (mpz_t *)malloc(1*sizeof(mpz_t))))
		return NULL;

	if (!(b = (mpz_t *)malloc(1*sizeof(mpz_t)))) {
		mpz_clear(*a), free(a);
		return NULL;
	}

	// always call init() on mpz_t, but don't call it twice
	mpz_init_set(*a, x), mpz_init_set(*b, y);

	while (mpz_sgn(*a) > 0 && mpz_sgn(*b) > 0)
		if (mpz_cmp(*b,*a) > 0) 
			mpz_mod(*b, *b, *a);
		else
			mpz_mod(*a, *a, *b);


	if (mpz_sgn(*a) == 0) {
		mpz_clear(*a), free(a);
		return b;
	} else {
		mpz_clear(*b), free(b);
		return a;
	}

}

// mcm = a*b/mcd(a,b)
// call lcm_free() on result
mpz_t * lcm(const mpz_t a, const mpz_t b) {

	mpz_t *md, *ab;

	if (!mpz_sgn(a) || !mpz_sgn(b) || !(md = gcd(a,b)))
		return NULL;

	if (mpz_sgn(*md) == 0) {
		gcd_free(md);
		return NULL;
	}

	if (!(ab = (mpz_t *)malloc(1*sizeof(mpz_t)))) {
		gcd_free(md);
		return NULL;
	}

	// always call init() on mpz_t
	mpz_init(*ab);
	mpz_mul(*ab, a, b);
	mpz_div(*ab, *ab, *md);

	gcd_free(md);

	return ab;
}


// call apply_free() on result
static mpz_t * apply(char **nums, int len, mpz_t*(*gfptr)(const mpz_t a, const mpz_t b), void (*gfree)(mpz_t *to_free) ) {

	mpz_t *vnum, *res;

       	if (!(res = (mpz_t *)malloc(1*sizeof(mpz_t))))
		return NULL;

	if (!(vnum = (mpz_t *)malloc(len*sizeof(mpz_t)))) {
		free(res);
		return NULL;
	}

	for (int i = 0; i < len; i++) {
		if (mpz_init_set_str(vnum[i], nums[i], 10) == -1) {
			for (int j = 0; j < i; j++)
				mpz_clear(vnum[j]);
			free(res);
			free(vnum);
			return NULL;
		}
	}

	mpz_t *to_free;
	mpz_init_set(*res, vnum[0]); // initialize and set *res

	// reuse the previous value of res, as argument for gfptr in the next iteration
	for (int i = 1; i < len; i++) {
		mpz_set(*res, *(to_free = gfptr(*res, vnum[i])));
		gfree(to_free);
	}


	for (int i = 0; i < len; i++)
		mpz_clear(vnum[i]);
	free(vnum);
	return res;
}

int main(int argc, char *argv[]) {

	void *fptr_purpose, *fptr_free;
	argv[0] = basename(argv[0]);

	if (argc < 3) {
_usage:
		fprintf(stderr, "usage: %s [-lcm|-gcd] [args..]\nexample: %s -gcd \"300\" \"360\"\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}


	if (strcmp(argv[1], "-gcd") == 0) {
		fptr_purpose = gcd;
		fptr_free    = gcd_free;
	} else if (strcmp(argv[1], "-lcm") == 0) {
		fptr_purpose = lcm;
		fptr_free    = lcm_free;
	} else {
		goto _usage;
	}


	for (int i = 2; i < argc; i++) {
		for (char *p = argv[i]; *p; p++) {
			if (!isdigit(*p)) {
				fprintf(stderr, "Invalid argument \"%s\"\n", argv[i]);
				return EXIT_FAILURE;
			}
		}
	}

	// skip progname & opt
	mpz_t *to_free = apply(argv+2, argc-2, fptr_purpose, fptr_free);

	printf("%s(", argv[1]+1); // skip '-' from opt
	for (int i = 2; i < argc; i++)
		printf("%s%s", argv[i], (i == argc-1 ? ") = " : ", "));
	mpz_out_str(stdout, 10, *to_free);
	puts("");

	apply_free(to_free);
	return EXIT_SUCCESS;

}
