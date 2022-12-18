# os2

A linux kernel module and a userspace program that communicates with it. (for Operating Systems class)

## os2-get usage

### Retrieving task_struct

Call program like:

```shell
os2-get -p <PID> 
```

Sends info to the kernel, returns what the kernel module prints.

### Retrieving net_device

Call program like:

```shell
os2-get -d <name> 
```

Sends info to the kernel, returns what the kernel module prints.

### Anything else

Errors out.
