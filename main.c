#include <stdio.h>
#undef NULL
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/* regs68k */
unsigned short getsr();
unsigned long getcacr();
unsigned long getsfc();
unsigned long setsfc(/*unsigned short*/);
unsigned long getdfc();
unsigned long setdfc(/*unsigned short*/);
unsigned long *getusp();
unsigned long *setusp(/*unsigned long **/);
unsigned long readphy(/*unsigned long []*/);
void writephy(/*const unsigned long [], unsigned long*/);

/* mmu030 */
void getcrp(/*unsigned long[2]*/);
void setcrp(/*const unsigned long[2]*/);
void getsrp(/*unsigned long[2]*/);
void setsrpfd(/*const unsigned long[2]*/);
unsigned long gettc();
void settcfd(/*unsigned long*/);
unsigned long gettt0();
unsigned long gettt1();
void settt0fd(/*unsigned long*/);
void settt1fd(/*unsigned long*/);

/* kernel */
void k2u(/*short sr, long pc, short type*/);

/* gestalt */
#include <mac/SysError.h>
#include <mac/Types.h>
#include <mac/OSUtils.h>
#define FOURCC(a,b,c,d) (((Long)(a)<<24)|((Long)(b)<<16)|((Long)(c)<<8)|((Long)(d)))

#define gestaltAddressingModeAttr FOURCC('a','d','d','r')
#define gestaltLogicalPageSize    FOURCC('p','g','s','z')
#define gestaltLogicalRAMSize     FOURCC('l','r','a','m')
#define gestaltLowMemorySize      FOURCC('l','m','e','m')
#define gestaltMMUType            FOURCC('m','m','u',' ')
#define gestaltNativeCPUType      FOURCC('c','p','u','t')
#define gestaltPhysicalRAMSize    FOURCC('r','a','m',' ')
#define gestaltProcessorType      FOURCC('p','r','o','c')
#define gestaltSysArchitecture    FOURCC('s','y','s','a')
#define gestaltVersion            FOURCC('v','e','r','s')
#define gestaltVMAttr             FOURCC('v','m',' ',' ')

OSErr Gestalt(/*OSType selector, Long *response*/);

/* mac */
#include <mac/Time.h>

#define BUF_SIZE 1048576L

static unsigned long *kwork;
static unsigned long *utable;
static unsigned long *kshm_base;

int main(argc, argv)
int argc;
char *argv[];
{
    short *mem;

    printf("Hello, 68k\n");

    printf("sizeof(short) = %d\n", sizeof(short));
    printf("sizeof(int)   = %d\n", sizeof(int));
    printf("sizeof(long)  = %d\n", sizeof(long));
    printf("\n");

    {
        unsigned short sr;
        unsigned long cacr;
        unsigned long sfc, dfc;

        sr = getsr();
        printf("sr = %04x\n", sr);
        cacr = getcacr();
        printf("cacr = %08lx\n", cacr);

#if 0
        sfc = setsfc(7);
        dfc = setdfc(7);
#else
        sfc = setsfc(5); /* Supervisor Data Space */
        dfc = setdfc(5); /* Supervisor Data Space */
#endif
        printf("pre sfc,dfc = %ld,%ld\n", sfc, dfc);
        sfc = getsfc();
        dfc = getdfc();
        printf("cur sfc,dfc = %ld,%ld\n", sfc, dfc);
    }
    printf("\n");
    
    printf("gestalt:\n");
    {
        OSErr err = noErr;
        long response;

        Gestalt(gestaltVersion, &response);
        printf("  Version: %ld\n", response);

        Gestalt(gestaltAddressingModeAttr, &response);
        printf("  AddressingMode: %ld\n", response);
/*
        Gestalt(gestaltLogicalPageSize, &response);
        printf("  Logical Page Size: %ld\n", response);
*/
        Gestalt(gestaltLogicalRAMSize, &response);
        printf("  Logical RAM Size: %ld\n", response);
        Gestalt(gestaltLowMemorySize, &response);
        printf("  Low Memory Size: %ld\n", response);

        Gestalt(gestaltMMUType, &response);
        printf("  MMU Type: %ld\n", response);
/*
        err = Gestalt(gestaltNativeCPUType, &response);
        printf("  Native CPU Type: %ld\n", response);
        printf("err = %d\n", err);
*/
        Gestalt(gestaltPhysicalRAMSize, &response);
        printf("  Physical RAM Size: %ld\n", response);
        utable = (unsigned long *)response - ((unsigned long)1<<(20-2)); /* 1MB */
        Gestalt(gestaltProcessorType, &response);
        printf("  Processor Type: %ld\n", response);
/*
        err = Gestalt(gestaltSysArchitecture, &response);
        printf("  Sys Architecture: %ld\n", response);
        printf("err = %d\n", err);
*/
        err = Gestalt(gestaltVMAttr, &response);
        printf("  VM: %ld\n", response);
        printf("err = %d\n", err);
    }
    printf("\n");

    /* 1MB heap */
    {
        unsigned long ptr;
        int i;

        mem = (short *)malloc(BUF_SIZE);
/*
        for (i = 0; i < 128; i += 8) {
            printf("%04x: ", i);
            printf("%04x %04x %04x %04x %04x %04x %04x %04x\n",
                mem[i+0], mem[i+1], mem[i+2], mem[i+3],
                mem[i+4], mem[i+5], mem[i+6], mem[i+7]);
        }
*/

        ptr = (unsigned long)mem;
        printf("begin = 0x%08lx\n", ptr);
        ptr += BUF_SIZE;
        printf("end   = 0x%08lx\n", ptr);
    }
    kwork = (unsigned long *)mem;
    kshm_base = (unsigned long *)(((unsigned long)kwork + BUF_SIZE - ((unsigned long)1<<15)) & 0xffffff00); /* 32KB, 256 aligned */

    /* MMU 030 */
    {
        unsigned long *p64;
        unsigned long r64hi;
        unsigned long r64lo;
        unsigned long r32;
        unsigned long ps;
        unsigned long *ptbl;

        p64 = (unsigned long *)mem;

        getcrp(p64);
        r64hi = *p64;
        r64lo = *(p64 + 1);
        printf("crp = 0x%08lx,0x%08lx\n", r64hi, r64lo);
        /* SE/30 24-bit mode default table */
        if (r64lo == 0x40800050) {
            setsrpfd(p64);
            settt0fd(0x02fd8543L); /* above 32MB,CI,R/W,Supervisor space */
            settt1fd(0x04fb8543L); /* above 64MB,CI,R/W,Supervisor space */
        }
        /* SE/30 32-bit mode default table */
        /* switched by SwapMMUMode (A05D)  */
        if (r64lo == 0x4083f5a0) {
            /* TODO */
        }
        getsrp(p64);
        r64hi = *p64;
        r64lo = *(p64 + 1);
        ptbl = (unsigned long *)(r64lo&0xfffffff0);
        printf("srp = 0x%08lx,0x%08lx\n", r64hi, r64lo);
        printf(" L/U: %ld\n", r64hi>>31);
        printf(" limit: %ld\n", (r64hi>>16)&0x7ffff);
        printf(" DT: %ld\n", r64hi&3);
        printf(" table addr.: 0x%08lx\n", ptbl);
        printf("  a[0]:  0x%08lx\n", readphy(ptbl+0));
        printf("  a[1]:  0x%08lx\n", readphy(ptbl+1));
        printf("  a[2]:  0x%08lx\n", readphy(ptbl+2));
        printf("  a[3]:  0x%08lx\n", readphy(ptbl+3));
        printf("  a[4]:  0x%08lx\n", readphy(ptbl+4));
        printf("  a[5]:  0x%08lx\n", readphy(ptbl+5));
        printf("  a[6]:  0x%08lx\n", readphy(ptbl+6));
        printf("  a[7]:  0x%08lx\n", readphy(ptbl+7));
        printf("  a[8]:  0x%08lx\n", readphy(ptbl+8));
        printf("  a[9]:  0x%08lx\n", readphy(ptbl+9));
        printf("  a[10]: 0x%08lx\n", readphy(ptbl+10));
        printf("  a[11]: 0x%08lx\n", readphy(ptbl+11));
        printf("  a[12]: 0x%08lx\n", readphy(ptbl+12));
        printf("  a[13]: 0x%08lx\n", readphy(ptbl+13));
        printf("  a[14]: 0x%08lx\n", readphy(ptbl+14));
        printf("  a[15]: 0x%08lx\n", readphy(ptbl+15));

        /* enable srp */
        r32 = gettc();
        r32 |= 0x02000000;
        settcfd(r32);
        ps = (unsigned long)1 << ((r32>>20)&15);
        printf("tcr = 0x%08lx\n", r32);
        printf(" E: %ld\n", r32>>31);
        printf(" SRE: %ld\n", (r32>>25)&1);
        printf(" FCL: %ld\n", (r32>>24)&1);
        printf(" PS : %ld[bits], %ld[bytes]\n", (r32>>20)&15, ps);
        printf(" IS : %ld\n", (r32>>16)&15);
        printf(" TIA: %ld\n", (r32>>12)&15);
        printf(" TIB: %ld\n", (r32>> 8)&15);
        printf(" TIC: %ld\n", (r32>> 4)&15);
        printf(" TID: %ld\n", (r32>> 0)&15);

#if 0
    {
        unsigned long secs;
        unsigned short result;

        /* A039 */
        result =  ReadDateTime(&secs);
        if (result != 0) {
            printf("error: %d\n", result);
        }
        printf("DateTime: %08lx\n", secs);
    }
#endif

        r32 = gettt0();
        printf("tt0 = 0x%08lx\n", r32);
        r32 = gettt1();
        printf("tt1 = 0x%08lx\n", r32);
    }
    printf("\n");

    /* create user process */
    {
        int i;
        unsigned long user_size = (unsigned long)1 << 20; /* 1MB */
        unsigned long user_phy;
        unsigned long entry;
        unsigned long *usp;
        int ret;
        struct stat fs;
        FILE *fp;
        long len;
        long l,r;
        unsigned long rmask;
        unsigned long data;

        printf("utable = 0x%08lx\n", utable);

        /* alloc user mem */
        user_phy = (unsigned long)utable - user_size;
        entry = (user_phy&0xffffff00) | 1; /* early termination page descriptor */
        writephy(&utable[0], entry);

        /* shared mem */
        entry = (unsigned long)&utable[2] | 2; /* short table descriptor */
        writephy(&utable[1], entry);
        /* table B */
        entry = (unsigned long)kshm_base | 1; /* normal page descriptor */
        writephy(&utable[2], entry);
        for (i = 1; i < 32; ++i) {
            entry = ((unsigned long)i<<8) /*marker*/ | 0; /* invalid */
            writephy(&utable[2+i], entry);
        }

        /* enable user memory */
        kwork[0] = 0x00010002; /* upper limit:1, DT:2(short) */
        kwork[1] = (unsigned long)utable & 0xfffffff0;
        setcrp(kwork);

        /* user stack */
        usp = (unsigned long *)(user_size); /* LA top */
        printf("usp top phy = 0x%08lx\n", user_phy + (unsigned long)usp);
        writephy(user_phy + (unsigned long)(--usp), 0xdeadbeef);
        writephy(user_phy + (unsigned long)(--usp), 0xcafebabe);
        writephy(user_phy + (unsigned long)(--usp), (unsigned long)1<<15);    /* shm size:32KB */
        writephy(user_phy + (unsigned long)(--usp), (unsigned long)kshm_base);/* kshm base */
        writephy(user_phy + (unsigned long)(--usp), user_size);               /* ushm base */

        usp = setusp(usp);
        printf("pre usp = 0x%08lx\n", usp);
        usp = getusp();
        printf("cur usp = 0x%08lx\n", usp);
        printf("\n");

        /* load user code */
        if (argc < 2) {
            fprintf(stderr, "no user code\n");
            return -1;
        }
        ret = stat(argv[1], &fs);
        if (ret < 0) {
            fprintf(stderr, "can't stat file: %s\n", argv[1]);
            return ret;
        }
        fp = fopen(argv[1], "rb");
        if (fp == (FILE *)NULL) {
            fprintf(stderr, "can't open user code\n");
            return -1;
        }
        len = fs.st_size;
        r = len & 3;
        len >>= 2;
        printf("user code size = (%ld << 2) + %ld\n", len, r);
        for (l = 0; l < len; ++l) {
            fread(&data, 1, sizeof(data), fp);
            writephy(user_phy + (l<<2), data);
        }
        if (r) {
            rmask = 0x80000000L >> ((r<<3) - 1);
            printf("before: %08lx, rmask: %08lx\n", data, rmask);
            len = fread(&data, 1, sizeof(data), fp);
            printf("after : %08lx, len: %ld\n", data, len);
            writephy(user_phy + (l<<2), data&rmask);
        }
        fclose(fp);

        /* user exec */
        printf("goto user land!\n");
        k2u(0x0000, 0x00000400L, 0x0000);
    }
    printf("\n");

    free(mem);

    return 0;
}
