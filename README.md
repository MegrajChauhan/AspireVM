## Aspire Virtual Machine

The Aspire Virtual Machine is a 64-bit virtual machine that doesn't emulate any real world architecture much like **`Java`**\. Aspire is currently immature as a 
VM\. It does, however, provide a basic instruction set **\(Incomplete at the time of writing this\)**\. The main purpose of this VM: the same as any other VM that I have created is to learn how computers work\.
**[Engima](https://github.com/MegrajChauhan/Enigma-Virtual-Machine), Enigma [v2](https://github.com/MegrajChauhan/Enigma-VM-Buffed) and [v3](https://github.com/MegrajChauhan/Enigma-VM), and [Khoya](https://github.com/MegrajChauhan/Khoya-VM)** all are my attempts in achieving this goal\.
There were other versions or VMs that I created that never made it to github\. The most special thing about Aspire is that it was the fastest and the best design
I have ever created\. The design or the implementation for every single one of the VM is almost the same but Aspire has been the most impressive one so far\.
Due to this, I decided to continue the development of this VM, adding more features and ultimately create a programming language that would compile for this VM\.

That goal is far off as I still haven't completed the intended instruction set\. More features needs to be added as well\. It's not like I am going without a plan\. 
There are plans that I have for this VM\. I have another VM underway namely, **ReeTry**\. The goal for that VM is to provide an emulated environment for Students to learn how Computers work\. 
ReeTry is aimed at emulating a CPU that doesn't exist **but** it will provide everything that a real, modern CPU would have\. The learners can write mock Operating Systems to learn how a Computer System is managed by a Software called Operating System\.
I have built a few prototypes and the most immature and incomplete one is on Github\. No progress has been made on it yet at the time of writing this\.

## How to learn about Aspire?

You can learn everything about Aspire in detail from the text files in the **`Details`** folder\.

## How to setup and run?

Well currently, the VM only runs on a 64-bit machine with a linux or similar system installed\. More support will be added in the future\.

**I am very shameful to mention this but even though I have programming for quite some time, I have never put in the efforts to learn Make or CMake or any such build systems. As such, I can only provide you with the command to compile and run the VM\.**

**There are no real programs to run yet, no assembler, no loader or anything like that\.** The code to be run has to be written in binary\. You can go to **`Aspire/Aspire.c`** in order to write the program\. The program should be written in the `program` array and the `PROGRAM_LEN` must be updated accordingly\.
The same goes for the `data` array and `DATA_LEN`\. Here is how you can compile the code:

1. Clone the repo first\(Since we only have the master branch\):
```bash
git clone https://github.com/MegrajChauhan/Aspire
```

2. Now write the program by updating **`Aspire/Aspire.c`**:
3. Now from the main folder, execute\(More files will be added and until I learn a build system or write a script, I will update this accordingly\):
```bash
gcc src/aspire_helpers.c src/lib/thread/aspire_threads.c src/aspire_instruction_memory.c src/aspire_memory.c src/aspire_cpu.c src/Manager/aspire_interrupt_handler.c src/Manager/aspire_manager.c Aspire/Aspire.c -O3 -o <The output folder name>/Aspire
```

## It is still immature!

As I have mentioned, Aspire is still a baby\. It can execute basic instructions but cannot provide support for file IO or normal IO, Networking, Multi-threading\(The foundations are complete for this one\)\. You can learn how to encode or write these instructions from the `Details` folder but until an Assembler is created, even I have to suffer\.