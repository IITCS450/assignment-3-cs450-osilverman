#include "types.h"
#include "stat.h"
#include "user.h"

#define NCHILDREN 3

volatile int counter[NCHILDREN];
static volatile int sink = 0;
static void burn(int n){ for(int i=0;i<n;i++) sink += i; }

int 
main(int argc, char **argv)
{
  int t = 10;
  if(argc == 2) t = atoi(argv[1]);

  // ===== PART 1: VALIDATION TESTS =====
  printf(1, "\n=== Part 1: settickets() Validation ===\n");
  
  if(settickets(0) == 0){
    printf(1, "FAIL: settickets(0) should fail\n");
    exit();
  }
  printf(1, "PASS: settickets(0) correctly returns -1\n");
  
  if(t >= 1 && settickets(t) != 0){
    printf(1, "FAIL: settickets(%d) should succeed\n", t);
    exit();
  }
  printf(1, "PASS: settickets(%d) correctly returns 0\n", t);
  
  // Burn some CPU
  for(int k=0; k<200; k++) burn(200000);
  printf(1, "PASS: settickets validation complete\n");

  // ===== PART 2: LOTTERY SCHEDULING EXPERIMENT =====
  printf(1, "\n=== Part 2: Lottery Scheduling Experiment ===\n");
  
  int tickets[NCHILDREN] = {10, 20, 30};  // Different ticket counts
  int i, pid;
  
  printf(1, "Setup:\n");
  printf(1, "  Child 0: %d tickets\n", tickets[0]);
  printf(1, "  Child 1: %d tickets\n", tickets[1]);
  printf(1, "  Child 2: %d tickets\n", tickets[2]);
  printf(1, "  Expected ratio: 1:2:3\n\n");
  
  int start_time = uptime();
  
  // Fork children
  for(i = 0; i < NCHILDREN; i++){
    counter[i] = 0;
    pid = fork();
    
    if(pid < 0){
      printf(1, "fork failed\n");
      exit();
    }
    
    if(pid == 0){
      // Child process
      settickets(tickets[i]);
      
      // CPU-bound work - count for a fixed time
      int work_start = uptime();
      volatile int work = 0;
      
      // Run for approximately 100 ticks
      while(uptime() - work_start < 100){
        work++;
      }
      
      printf(1, "Child %d (PID %d, %d tickets): work = %d\n", 
             i, getpid(), tickets[i], work);
      exit();
    }
  }
  
  // Parent waits for all children
  for(i = 0; i < NCHILDREN; i++){
    wait();
  }
  
  int end_time = uptime();
  printf(1, "\nTotal runtime: %d ticks\n", end_time - start_time);
  printf(1, "\nAnalysis: Compare work values above.\n");
  printf(1, "They should be approximately in ratio 1:2:3\n");
  printf(1, "(Child 1 should do ~2x Child 0's work)\n");
  printf(1, "(Child 2 should do ~3x Child 0's work)\n");
  
  printf(1, "\ntestlottery: ALL TESTS COMPLETE\n\n");
  exit();
}