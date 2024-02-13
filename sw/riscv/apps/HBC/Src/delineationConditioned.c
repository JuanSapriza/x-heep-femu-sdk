#include "delineationConditioned.h"
#include "defines.h"
#include "morpho_filtering.h"
#include "rms.h"
#include "peakDetection.h"
#include "relativeEnergy.h"
#include "rp_classifier.h"

#include "profiling/profile.h"
// #include "profiling/defines.h"

#include <stdio.h>


#define N_WINDOWS (ECG_VECTOR_SIZE/dim)

PI_L2 int16_t ecg_buff[dim*(NLEADS+1)];
PI_L2 int32_t indicesRpeaks[H_B+1];
PI_L2 int32_t indicesBeatClasses[H_B+1];
PI_L2 uint32_t complete_del[H_B * FPSIZE];

PI_L2 int32_t *arg[12];

PI_L2 int32_t count_window;
PI_L2 int32_t overlap;
PI_L2 int32_t buffSize_MF_RMS;
PI_L2 int32_t flag_prevWindAB;
PI_L2 int32_t rWindow;


void clearRelEn() {
    clearAndResetRelEn();
    resetPeakDetection();
}

void delineateECG(){


#ifdef MODULE_MF_3L

    #ifdef HWPERF_MODULES
            profile_start();
    #endif

        if(flag_prevWindAB==1) {
            arg[0] = (int32_t*) ecg_buff;
            buffSize_MF_RMS = dim;
        } else {
            arg[0] = (int32_t*) &ecg_buff[overlap];
            buffSize_MF_RMS = dim-overlap;
        }

        for(int32_t lead_abnbeat = 1; lead_abnbeat < NLEADS; lead_abnbeat++){
            arg[9] = &lead_abnbeat;
            filterWindows(arg);
        }

    #ifdef HWPERF_MODULES
            profile_stop();
    #endif

    #ifdef PRINT_SIG_MF_3L
        if(count_window==0) {
            int32_t lead_print = 1;
            for(int32_t sample = dim*lead_print; sample<dim*(lead_print+1); sample++) {
                printf("%d ", ecg_buff[sample]);
            }
            printf("\n");
        }
    #endif

#endif

#ifdef MODULE_RMS_3L

    #ifdef HWPERF_MODULES
            profile_start();
    #endif

            arg[0] = (int32_t*) ecg_buff;
            arg[1] = (int32_t*) &ecg_buff[dim*NLEADS]; //Using last dim samples of the buffer to keep the first for the MF lead0
            buffSize_MF_RMS = dim;

            combine_leads(arg);

    #ifdef HWPERF_MODULES
            profile_stop();
    #endif

    #ifdef PRINT_SIG_RMS_3L
            if(count_window==0) {
                for(int32_t sample = dim*NLEADS; sample<dim*(NLEADS+1); sample++) {
                    printf("%d ", ecg_buff[sample]);
                }
                printf("\n");
            }
    #endif

#endif



#ifdef MODULE_DEL_3L

            arg[0] = (int32_t*) &ecg_buff[dim*NLEADS];

    #ifdef HWPERF_MODULES
            profile_start();
    #endif
            delineateECG_w(arg);

    #ifdef HWPERF_MODULES
            profile_stop();
    #endif

#endif
}

void classifyBeatECG()  {

    int32_t count_sample = 0;
    int32_t firstDel = 0;
    int32_t offset_del = 0;
    int32_t rpeaks_counter = 0;
    int32_t tot_overlap = 0;
    int32_t offset_MF = 0;
    int32_t totP = 0;
    int32_t flagMF = 0;
    int32_t i_lead = 0;
    int32_t flag_abnBeat = 0;

    if(ECG_SAMPLING_FREQUENCY == 250){
        offset_MF = 150;
    } else {
        offset_MF = 300;
    }

    buffSize_MF_RMS = dim;
    count_window = 0;
    overlap = 0;
    flag_prevWindAB = 0;

    arg[0] = (int32_t*) ecg_buff;
    arg[1] = (int32_t*) &ecg_buff[dim];
    arg[2] = indicesRpeaks;
    arg[3] = &offset_del;
    arg[4] = &rpeaks_counter;
    arg[5] = (int32_t*) complete_del;
    arg[6] = &totP;
    // arg[7] = &rWindow;
    arg[8] = &flagMF;
    arg[9] = &i_lead;
    arg[10] = &buffSize_MF_RMS;
    arg[11] = indicesBeatClasses;

    clearRelEn();

    for(rWindow=0; rWindow<N_WINDOWS; rWindow++)
    {
        printf("Win #%d\n", rWindow);
        
#ifdef HWPERF_FULL
        profile_start();
#endif

        if (firstDel == 1) {
            for(int32_t lead=0; lead<NLEADS; lead++) {
                for(int32_t i=0; i<overlap; i++) {
                    ecg_buff[i + dim*lead] = ecg_buff[(dim - overlap + i) + dim*lead]; //copy the overlap of the 3 leads
                }
            }
        }

        /**
         * The data is always read in timelime order with ascending lead count
         */
        for(int32_t i=overlap; i<dim; i++) {
            for(int32_t lead=0; lead<NLEADS; lead++) {
                read_vadc_dma( &ecg_buff[i + dim*lead] , sizeof(uint16_t) );
            }
        }

        if (firstDel == 0) {
            // Needed to initialize the MF filter properly
            for(int32_t lead=0; lead<NLEADS; lead++) {
                for(int32_t i=0; i<=offset_MF; i++) {
                    ecg_buff[i + dim*lead] = 0;
                }
            }
        }

#ifdef HWPERF_1L
        profile_start();
#endif

#ifdef MODULE_MF

    #ifdef HWPERF_MODULES
        profile_start();
    #endif

        arg[0] = (int32_t*) &ecg_buff[overlap];
        buffSize_MF_RMS = dim-overlap;

        arg[9]= &i_lead;
        filterWindows(arg);

    #ifdef HWPERF_MODULES
        profile_stop(); 
    #endif

    #ifdef PRINT_SIG_MF
        if(count_window==0){
            for(int32_t sample = 0; sample<dim; sample++) {
                printf("%d ", ecg_buff[sample]);
            }
            printf("\n");
        }
    #endif

#endif // MODULE_MF


#ifdef MODULE_RELEN

    #ifdef HWPERF_MODULES
        profile_start();
    #endif
        clearAndResetRelEn();

        arg[0] = (int32_t*) ecg_buff; //keep the first dim samples for lead0 MF
        arg[1] = (int32_t*) &ecg_buff[dim*NLEADS];

        relEn_w(arg);

    #ifdef HWPERF_MODULES
        profile_stop();
    #endif

    #ifdef PRINT_RELEN
        if(count_window==0) {
            for(int32_t sample = dim*NLEADS; sample<dim*(NLEADS+1); sample++) {
                printf("%d ", ecg_buff[sample]);
            }
            printf("\n");
        }
    #endif

#endif // MODULE_RELEN

#ifdef MODULE_RPEAK

    #ifdef HWPERF_MODULES
        profile_start();
    #endif
        getPeaks_w(arg);

        rpeaks_counter = 0;

        while(indicesRpeaks[rpeaks_counter]!=0) {
            rpeaks_counter++;
        }

    #ifdef HWPERF_MODULES
        profile_stop();
    #endif

    #ifdef PRINT_RPEAKS
        for(int32_t indR=0; indR<rpeaks_counter; indR++) {
            printf("%d ", (indicesRpeaks[indR] + offset_del));
        }
        printf("\n");
    #endif

#endif // MODULE_RPEAK


#ifdef MODULE_BEATCLASS

    #ifdef HWPERF_MODULES
        profile_start();
    #endif
        report_rpeak(arg);
        for(int32_t indR=0; indR<rpeaks_counter; indR++) {
            if(indicesBeatClasses[indR]>0) {
                flag_abnBeat=1;
                break;
            }
        }
    #ifdef HWPERF_MODULES
        profile_stop();
    #endif

    #ifdef PRINT_BEATCLASS
        for(int32_t indR=0; indR<rpeaks_counter; indR++) {
          printf("%d %d\n", indicesRpeaks[indR], indicesBeatClasses[indR]);
        }
    #endif
    
    #ifdef PRINT_RESULT
        if(flag_abnBeat)
	    printf("Window %d: ABNORMAL_BEAT!\n", rWindow);
	else
	    printf("Window %d: BIEN!\n", rWindow);
    #endif

#endif // MODULE_BEATCLASS

#ifdef HWPERF_1L
        profile_stop();
#endif


#ifdef MODULE_3L

#ifdef HWPERF_3L
        profile_start();
#endif
        if(flag_abnBeat==1) {
            delineateECG();
            flag_abnBeat = 0;
            flag_prevWindAB = 0;
        } else {
            flag_prevWindAB = 1;
        }

#ifdef HWPERF_3L
        profile_stop();
#endif

#endif

#ifdef HWPERF_FULL
        profile_stop();
#endif

    #ifdef PRINT_DEL
            for(int32_t ix = 0; ix<(rpeaks_counter-1)*FPSIZE; ix++)
                printf("%d ", complete_del[ix]);
            printf("\n");
    #endif

#ifdef ONLY_FIRST_WINDOW
        return;
#endif

#ifdef ONLY_TWO_WINDOW
        if (rWindow == 1)
            return;
#endif

        overlap = dim - (indicesRpeaks[rpeaks_counter - 2] - LONG_WINDOW);


#ifdef DEBUG_FIRST_MODULES
        overlap = LONG_WINDOW+LONG_WINDOW;
#endif

        tot_overlap += overlap;
        offset_del = (count_window+1) * dim - tot_overlap;

        totP += rpeaks_counter-1;

        firstDel = 1;
        flagMF=1;
        count_window++;
        rpeaks_counter = 0;

        for(int32_t ix_rp = 0; ix_rp < H_B+1 ; ix_rp++) {
           indicesRpeaks[ix_rp] = 0;
        }

    }
}
