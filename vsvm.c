#include "vsvm.h"
#include "instruction_set.h"
#include <stdio.h>

struct vm_state {
    int *instructions;
    int program_counter;
    int *stack;
    int stack_pointer;
    int frame_pointer;
    int is_running;
};

void print_stack(struct vm_state *state) {
    printf("    [stack]");
    for (int i = 0; i <= state->stack_pointer; i++) {
        printf("->%d", state->stack[i]);
    }
    printf("\n");
}

int fetch(struct vm_state *state) {
    return state->instructions[state->program_counter++];
}

int decode(int instruction) {
    return instruction;
}

int pop(struct vm_state *state) {
    return state->stack[state->stack_pointer--];
}

int push(struct vm_state *state, int value) {
    return (state->stack[++state->stack_pointer] = value) == value;
}

int peek(struct vm_state *state) {
    return state->stack[state->stack_pointer];
}

int execute(struct vm_state *state, int decoded_instruction) {
    int ret_val = 1;
    print_stack(state);
    if (decoded_instruction == VERYSIMPLE_VM_ICONST) {
        int constant = state->instructions[state->program_counter++];
        push(state, constant);
        printf("iconst %d\n", constant);
    } else if (decoded_instruction == VERYSIMPLE_VM_IADD) {
        int b = pop(state);
        int a = pop(state);
        push(state, a + b);
        printf("iadd %d %d\n", a, b);
    } else if (decoded_instruction == VERYSIMPLE_VM_ISUB) {
        int b = pop(state);
        int a = pop(state);
        push(state, a - b);
        printf("isub %d %d\n", a, b);
    } else if (decoded_instruction == VERYSIMPLE_VM_IMUL) {
        int b = pop(state);
        int a = pop(state);
        push(state, a * b);
        printf("imul %d %d\n", a, b);
    } else if (decoded_instruction == VERYSIMPLE_VM_LT) {
        int b = pop(state);
        int a = pop(state);
        push(state, a < b);
        printf("lt %d %d\n", a, b);
    } else if (decoded_instruction == VERYSIMPLE_VM_GT) {
        int b = pop(state);
        int a = pop(state);
        push(state, a > b);
        printf("gt %d %d\n", a, b);
    } else if (decoded_instruction == VERYSIMPLE_VM_EQ) {
        int b = pop(state);
        int a = pop(state);
        push(state, a == b);
        printf("eq %d %d\n", a, b);
    } else if (decoded_instruction == VERYSIMPLE_VM_NEQ) {
        int b = pop(state);
        int a = pop(state);
        push(state, a != b);
        printf("neq %d %d\n", a, b);
    } else if (decoded_instruction == VERYSIMPLE_VM_BRT) {
        int location = state->instructions[state->program_counter++];
        int flag = pop(state);
        printf("brt %d %d\n", flag, location);
        if (flag) {
            state->program_counter = location;
        }
    } else if (decoded_instruction == VERYSIMPLE_VM_BRF) {
        int location = state->instructions[state->program_counter++];
        int flag = pop(state);
        printf("brf %d %d\n", flag, location);
        if (!flag) {
            state->program_counter = location;
        }
    }  else if (decoded_instruction == VERYSIMPLE_VM_CALL) {
        printf("call\n");
    }
    else if (decoded_instruction == VERYSIMPLE_VM_RET) {
        printf("ret\n");
    }else if (decoded_instruction == VERYSIMPLE_VM_PRINT) {
        printf("print %d\n", peek(state));
    } else if (decoded_instruction == VERYSIMPLE_VM_HALT) {
        state->is_running = 0;
        printf("halt\n");
    } else {
        printf("Unknown instruction %d\n", decoded_instruction);
        ret_val = 0;
    }
    print_stack(state);
    return ret_val;
}

void run(int *instructions, int instructions_size, int entry_point) {
    int stack[100];
    struct vm_state state = {
            .stack = &stack,
            .stack_pointer = -1,
            .program_counter = entry_point,
            .instructions = instructions,
            .frame_pointer = -1,
            .is_running = 1
    };
    int is_execution_successful = 1;
    while (is_execution_successful && state.is_running && state.program_counter < instructions_size) {
        int instruction = fetch(&state);
        int decoded_instruction = decode(instruction);
        is_execution_successful = execute(&state, decoded_instruction);
    }
    return;
}