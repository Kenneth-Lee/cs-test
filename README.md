# context switch test set

* ci: get the time unit to do memory write.
* cs: use queue to manage the workload of a set of threads to do memory write.
* malloc\_test: malloc and set the memory from a set of threads.
* amdahl: multi-provider multi-consumer model on a queue with different yiled method

* ctx\_switch: switch processes in a bound cpu in fixed frequence

  * ctx\_switch\_test.sh: batch test on different number of processes.
    need to change according to the requirement and platform. run as:
    ```sh
      ctx_switch_test.sh 2>&1 | tee test.result
    ```

  * render\_ctx\_switch\.py: render the ctx\_switch\_test.sh perf result. run as:
    ```sh
      render_ctx_switch.py result1 result2...
    ```
