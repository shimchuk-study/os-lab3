#!/usr/bin/env bash

./os_lab3 --mem_size=65536 \
          --proc_gen_cycles=40 \
          --wait=20 \
          --min_proc_mem=1024 \
          --range_proc_mem=15364 \
          --proc_gen_tries=2 \
          --mem_access_tries=2 \
          --prob_proc_creation=0.5 \
          --min_prob_exit=0.1 \
          --range_prob_exit=0.8 \
          --min_prob_memaccess=0.5 \
          --range_prob_memaccess=0.4
