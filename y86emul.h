int mypow(int base, int exp);
int convert_to_num(unsigned char c) ;
int convert_to_dec(const char* in);
int execute(unsigned char * mem);

void rrmovl(unsigned char* mem);
void irmovl(unsigned char* mem);
void rmmovl(unsigned char* mem);
void mrmovl(unsigned char* mem);
void op1(unsigned char* mem, int func);
void jxx (unsigned char* mem, int func);
void call(unsigned char* mem);
void ret(unsigned char* mem);
void pushl(unsigned char* mem);
void popl(unsigned char* mem);
void readx(unsigned char* mem, int func);
void writex(unsigned char* mem, int func);
void movsbl(unsigned char* mem);
void write_int_be(unsigned char* mem, int val, int index);
void write_int(unsigned char* mem, int val, int index);

int read_int(unsigned char* mem);
int read_int_mem(unsigned char* mem, int index);
