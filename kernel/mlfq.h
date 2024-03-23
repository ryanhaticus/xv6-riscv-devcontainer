// Using a header guard as this file is shared between the user and kernel space.
// Preventing any mishaps with imports.
#ifndef MLFQ_H
#define MLFQ_H

// Implementing the MLFQInfoReport structure, as required by Project 1C.
#define MLFQ_MAX_LEVEL 10
struct MLFQInfoReport {
  int tickCounts[MLFQ_MAX_LEVEL];
};

// A structure to be used within the PCB to store MLFQ-specific information, as required by Project 1C.
struct MLFQProcInfo {
  int queued;
  int priorityLevel;
  struct MLFQInfoReport report; // Ticks ran on each level.
  int ticksAtMaxLevel; // Ticks the process has been queued on level m-1.
  int ticksAtCurrentLevel; // Ticks the process has been running on current level.
};

#endif