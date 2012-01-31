/*-----------------------Patrick 14/11/96--------------------------
  Header for AVP Pheromone system
  -----------------------------------------------------------------*/


/*  Global Data access to pheromone system */

extern unsigned int *PherPl_ReadBuf;
extern unsigned int *PherPl_WriteBuf;
extern unsigned int *PherAls_ReadBuf;
extern unsigned int *PherAls_WriteBuf;
extern unsigned int *PherMars_ReadBuf;
extern unsigned int *PherMars_WriteBuf;
extern unsigned char *PherAi_Buf;
extern MODULE *playerPherModule;

/* function prototypes */
void InitPheromoneSystem(void);
void PlayerPheromoneSystem(void);
void AiPheromoneSystem(void);
void CleanUpPheromoneSystem(void);
int AIModuleAdmitsPheromones(AIMODULE *targetModule);

void AddMarinePheromones(AIMODULE *targetModule);
void MaintainMarineTargetZone(AIMODULE *targetModule);

