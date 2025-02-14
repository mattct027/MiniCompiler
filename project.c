#include "spimcore.h"

//Group Members: Bentley Aing Matthew Thomas

/* ALU */
/* 10 Points */
void ALU(unsigned A, unsigned B, char ALUControl, unsigned *ALUresult, char *Zero)
{

    //add case
    if (ALUControl == 0) { 
        *ALUresult = A + B;
    }

    //subtract case
    else if (ALUControl == 1) { 
        *ALUresult = A - B;
    }

    //slt case
    else if (ALUControl == 2) { 
        *ALUresult = (int)A < (int)B ? 1 : 0;;
    }

    //sltu case
    else if (ALUControl == 3) { 
        *ALUresult = (unsigned)A < (unsigned)B ? 1 : 0;
    }

    //and case
    else if (ALUControl == 4) {
        *ALUresult = (A & B);
    }

    //or case
    else if (ALUControl == 5) {
        *ALUresult = (A | B);
    }

    //shift left case
    else if (ALUControl == 6) { 
        *ALUresult = B << 16;
    }

    //not case
    else if (ALUControl == 7) { 
        *ALUresult = ~A;
    }

    // set the zero flag based on alu result
    *Zero = (*ALUresult == 0) ? 1 : 0;
}


/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    //halt condition check, needs to be a multiple of 4 and in bounds
    if (PC % 4 != 0 || PC > 65536) 
        return 1;

    //instruction fetch
        *instruction = Mem[PC >> 2]; //shift right by 2 to get the correct index (does pc / 4)
        return 0;
}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    *op = (instruction & 0xfc000000) >> 26; // 31-26
        //bits 31-26 set to 1 for masking 

    *r1 = (instruction & 0x03e00000) >> 21; // 25-21
        //bits 25-21 set to 1 for masking

    *r2 = (instruction & 0x1f0000) >> 16; // 20-16
        //bits 20-16 set to 1 for masking

    *r3 = (instruction & 0xf800) >> 11; // 15-11
        //bits 15-11 set to 1 for masking

    *funct = instruction & 0x3f; // 5-0
        //bits 5-0 set to 1 for masking
    *offset = instruction & 0xffff;	 // 15-0
        //bits 15-0 set to 1 for masking
    *jsec = instruction & 0x3ffffff; // 25-0
        //bits 25-0 set to 1 for masking
}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
     controls->RegDst = 0;
     controls->Jump = 0;
     controls->Branch = 0;
     controls->MemRead = 0;
     controls->MemtoReg = 0;
     controls->ALUOp = 0;
     controls->MemWrite = 0;
     controls->ALUSrc = 0;
     controls->RegWrite = 0;

     if(op == 0){ //r-type insturction
        controls->RegDst = 1;
        controls->RegWrite = 1;
        controls->ALUOp = 7;

     }
     else if(op == 2){ //jump instruction
   
        controls->Jump = 1;
        

     }
     else if(op == 8 || op == 12){ //addi and andi
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
        controls->ALUOp = 0;
        if(op == 12){
            controls->ALUOp = 4;
        }
     }
     else if(op == 4){//beq
        controls->RegDst = 2;
        controls->Branch = 1;
        controls->MemtoReg = 2;
        controls->ALUOp = 1;
     } 
     else if(op == 5){ //bne
        controls->RegDst = 2;
        controls->MemtoReg = 2;
        controls->ALUSrc = 1;
     }
     else if(op == 35){ //loadword
        controls->MemRead = 1;
        controls->MemtoReg = 1;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
     }
     else if(op == 43){ //storeword
        controls->RegDst = 2; 
        controls->MemWrite = 1;
        controls->MemtoReg = 2; 
        controls->ALUSrc = 1;
     }
     else if(op == 10 || op == 11){ //slt, sltu
        
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
        controls->ALUOp = 2;
        if(op == 11){
            controls->ALUOp = 3;
        }
     }
    else if (op == 15){ // lui
        controls->RegWrite = 1;
        controls->ALUSrc = 1;
        controls->ALUOp = 6;
    }
    else
         return 1;
     return 0;


     // if else chain based off of value of op to set control signals

}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{

        if((offset >> 15) == 1) // could also use offset & 0x8000
            *extended_value = offset | 0xffff0000; //upper 16 bits

        else
            *extended_value = offset & 0x0000ffff; // lower 16 bits
}

/* ALU operations */
/* 10 Points */

int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    char ALUControl = 0;

    if (ALUSrc == 1) { // fixed 2c -> 30 now 2c -> 24
        data2 = extended_value;
    }

if (ALUOp == 7) {
    if (funct == 0) { // shift left
        ALUControl = 6;
    } else if (funct == 32) { // addition
        ALUControl = 0;
    } else if (funct == 34) { // subtraction
        ALUControl = 1;
    } else if (funct == 36) { // and
        ALUControl = 4;
    } else if (funct == 37) { // or
        ALUControl = 5;
    } else if (funct == 39) { // not (nor)
        ALUControl = 7;
    } else if (funct == 42) { // set less than
        ALUControl = 2;
    } else if (funct == 43) { // set less than unsigned
        ALUControl = 3;
    } else {
        return 1; // invalid funct
    }

    ALU(data1, data2, ALUControl, ALUresult, Zero);
    return 0;
} 
    //Seperate type of instruction for R-type
    else  {
        ALU(data1, data2, ALUOp, ALUresult, Zero);
        return 0;
}

return 1; // invalid 

}


/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    if(MemRead == 1) {
        if(ALUresult % 4 != 0 || ALUresult > 65536) // check check for halt condition ?may need to change to encompass out of bounds
            return 1;
        *memdata = Mem[ALUresult >> 2]; // read from memory
    }
    if(MemWrite == 1) {
        if(ALUresult % 4 != 0 || ALUresult > 65536) // check for halt condition  ?may need to change to encompass out of bounds
            return 1;
        Mem[ALUresult >> 2] = data2; // write to memory
    }
    return 0;
}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    if(RegWrite == 1){
        if(MemtoReg == 1){
            Reg[r2] = memdata; //load word
        }
        else if(MemtoReg == 0){
            //R-Type or I-Type
            if(RegDst == 1){
                Reg[r3] = ALUresult; 
            }else{
                Reg[r2] = ALUresult;
            }
        }
    }
}

/* PC update */
/* 10 Points */

void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    *PC += 4; // increment PC by 4

    if(Branch == 1 && Zero == 1){
        *PC += (extended_value << 2); // branch
    }

    if(Jump == 1){
        *PC =(jsec << 2) | (*PC & 0xf0000000); // jump
    }
}

