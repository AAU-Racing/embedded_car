#define UPDATEREADY 0xA8
#define UPDATEACK 0xA9

#define STARTADDRESS 0x08010000

#define FOUR(x)						\
      (((int) (x[0] & 0x07)) << 18)	\
    | (((int) (x[1] & 0x3F)) << 12)	\
    | (((int) (x[2] & 0x3F)) <<  6)	\
    | (((int) (x[3] & 0x3F)))

#define THREE(x) 					\
      (((int) (x[0] & 0x0F)) << 12)	\
    | (((int) (x[1] & 0x3F)) << 16)	\
    | (((int) (x[2] & 0x3F)))

#define TWO(x)						\
      (((int) (x[0] & 0x3F)) <<  6)	\
    | (((int) (x[1] & 0x3F)))

typedef struct Packet
{
    uint8_t id;
    uint8_t length;
    uint8_t *payload;
} Packet;

typedef enum comType { USB = 0, UART, CAN} ComType;

bool LookForUpdate(void);
void SetComType(ComType);
void RecievePacket(Packet*);
void SendPacket(Packet);
int convert(int* x, int length);
