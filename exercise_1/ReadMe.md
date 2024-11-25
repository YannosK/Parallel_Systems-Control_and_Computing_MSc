# TO-DO

## Report
- [ ] system info
    - [ ] name of system
    - [ ] CPU
    - [ ] number of cores
    - [ ] OS version
    - [ ] compiler version 
    - [ ] cache line size
- [ ] always mention the inputs
- [ ] always execute at least 5 times and report average

# Charon info

- with `getconf LEVEL1_DCACHE_LINESIZE` I found that the system has cache line size 64 bytes

# Questions

- Should we use barriers to time the threads?