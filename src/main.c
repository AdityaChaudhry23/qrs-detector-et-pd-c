/*-------------------------------------------------------
 *  main.c
 *  Read MIT‑BIH record 203 with WFDB, run:
 *      1. 5–15 Hz FIR band‑pass  (101 taps, Hamming)
 *      2. Pan‑Tompkins derivative
 *  Write the two vectors to:
 *      results/203_bandpass.dat
 *      results/203_derivative.dat
 *
 *  Build example (adjust paths to your tree / Makefile):
 *      gcc -O3 -std=c17 \
 *          -Iincludes -Iexternal/wfdb/include \
 *          src/main.c src/qrs_detector.c \
 *          -Lexternal/wfdb/lib -lwfdb -lm \
 *          -o build/qrs_detector
 *------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <wfdb/wfdb.h>

#include "qrs_detector.h"          /* holds prototypes you pasted   */

#define DATA_DIR     "data"        /* where 203.hea / 203.dat live  */
#define RESULT_DIR   "results"
#define RECORD       "203"

#define FS           360.0         /* sampling frequency (Hz)       */
#define NTAPS        101           /* FIR length (odd)              */

/* ------------ helpers -------------------------------- */
static int load_record_wfdb(const char *rec,
                            const char *dir,
                            double    **buf,
                            size_t     *len)
{
    setwfdb(dir);

    WFDB_Siginfo si[WFDB_MAXSIG];
    int nsig = isigopen(rec, si, 0);
    if (nsig < 1) { fprintf(stderr,"isigopen %s failed\n",rec); return -1; }

    long N = si[0].nsamp;
    if (N <= 0) { fprintf(stderr,"nsamp unknown\n"); isigclose(); return -1; }

    WFDB_Sample *v = malloc(nsig * sizeof *v);
    double *out     = malloc((size_t)N * sizeof *out);
    if (!v || !out) { perror("malloc"); isigclose(); return -1; }

    for (long i = 0; i < N; ++i) {
        if (getvec(v) <= 0) { N = i; break; }
        out[i] = (double)(v[0] - si[0].baseline) / si[0].gain;  /* mV */
    }
    free(v); isigclose();
    *buf = out; *len = (size_t)N;
    return 0;
}

static int dump_vector(const char *path,
                       const double *v,
                       size_t n)
{
    FILE *fp = fopen(path,"w");
    if (!fp) { perror(path); return -1; }
    for (size_t i=0;i<n;++i) fprintf(fp,"%.18e\n",v[i]);
    fclose(fp);
    return 0;
}

/* ------------ main ----------------------------------- */
int main(void)
{
    /* 1. load raw ECG */
    double *raw = NULL; size_t N = 0;
    if (load_record_wfdb(RECORD, DATA_DIR, &raw, &N)) return 1;

    /* 2. allocate working buffers */
    double *bp  = calloc(N, sizeof *bp);
    double *der = calloc(N, sizeof *der);
    if (!bp || !der) { perror("calloc"); free(raw); return 2; }

    /* 3. band‑pass (your signature: input, output, len, taps, fs) */
    bandpass_filter(raw, bp, (int)N, NTAPS, FS);

    /* 4. derivative */
    if (derivative_filter(bp, der, N)) {
        fprintf(stderr,"derivative error\n"); goto cleanup;
    }

    /* 5. save results */
    char f1[64], f2[64];
    snprintf(f1,sizeof f1,"%s/%s_bandpass.dat",  RESULT_DIR, RECORD);
    snprintf(f2,sizeof f2,"%s/%s_derivative.dat",RESULT_DIR, RECORD);
    if (dump_vector(f1, bp ,N) ||
        dump_vector(f2, der,N)) goto cleanup;

    printf("✅  %s processed – %zu samples written to '%s/'\n",
           RECORD, N, RESULT_DIR);

cleanup:
    free(raw); free(bp); free(der);
    return 0;
    printf("Hello");
}
