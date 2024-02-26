#include "profiling/profile.h"	

// PULP SDK v2 (latest)
void profile_start()	{
	
	printf("\nstart profile\n");
	
#ifdef ACTIVE
	pi_perf_conf((1<<PI_PERF_CYCLES) | (1<<PI_PERF_ACTIVE_CYCLES));
#endif
#ifdef STALL
	pi_perf_conf((1<<PI_PERF_LD_STALL) | (1<<PI_PERF_IMISS));
#endif
#ifdef EXTACC
	pi_perf_conf((1<<PI_PERF_LD_EXT_CYC) | (1<<PI_PERF_ST_EXT_CYC));
#endif
#ifdef INTACC
	pi_perf_conf((1<<PI_PERF_LD) | (1<<PI_PERF_ST));
#endif
#ifdef INSTRUCTION
	pi_perf_conf((1<<PI_PERF_INSTR));
#endif
#ifdef TCDM
	pi_perf_conf((1<<PI_PERF_TCDM_CONT));
#endif
#ifdef JUMPS
    pi_perf_conf( (1<<PI_PERF_JUMP) );
#endif
#ifdef BRANCHES
    pi_perf_conf( (1<<PI_PERF_BRANCH) );
#endif
#ifdef TAKEN_BRANCHES
    pi_perf_conf( (1<<PI_PERF_BTAKEN) );
#endif
	pi_perf_reset();
	pi_perf_start();
}


void profile_stop()	{
	    
	int id = pi_core_id();
    
	pi_perf_stop();

#ifdef ACTIVE
	printf("[%d] cycles = %d\n", id, pi_perf_read (PI_PERF_CYCLES));
	printf("[%d] active cycles = %d\n", id, pi_perf_read (PI_PERF_ACTIVE_CYCLES));
#endif
#ifdef STALL
	printf("[%d] LD stall = %d\n", id, pi_perf_read (PI_PERF_LD_STALL));
	printf("[%d] IMISS = %d\n", id, pi_perf_read (PI_PERF_IMISS));
#endif
#ifdef EXTACC
	printf("[%d] PI_PERF_LD_EXT_CYC = %d\n", id, pi_perf_read (PI_PERF_LD_EXT_CYC));
	printf("[%d] PI_PERF_ST_EXT_CYC = %d\n", id, pi_perf_read (PI_PERF_ST_EXT_CYC));
#endif
#ifdef INTACC
	printf("[%d] PI_PERF_LD_CYC = %d\n", id, pi_perf_read (PI_PERF_LD));
	printf("[%d] PI_PERF_ST_CYC = %d\n", id, pi_perf_read (PI_PERF_ST));
#endif
#ifdef INSTRUCTION
	printf("[%d] instr = %d\n", id, pi_perf_read (PI_PERF_INSTR));	
#endif
#ifdef TCDM
	printf("[%d] PI_PERF_TCDM_CONT = %d\n", id, pi_perf_read (PI_PERF_TCDM_CONT));	
#endif
#ifdef JUMPS
    printf("[%d] Unconditional jumps = %d\n", id, pi_perf_read (PI_PERF_JUMP));
#endif
#ifdef BRANCHES
    printf("[%d] Total branches = %d\n", id, pi_perf_read (PI_PERF_BRANCH));
#endif
#ifdef TAKEN_BRANCHES
    printf("[%d] Branches taken = %d\n", id, pi_perf_read (PI_PERF_BTAKEN));
#endif
}
