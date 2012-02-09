/* Plat_Shp.h  */


extern int Request_PolyFlags(void *polygon);

extern int SetupPolygonAccess(DISPLAYBLOCK *objectPtr);
extern void AccessNextPolygon(void);
extern void GetPolygonVertices(struct ColPolyTag *polyPtr);
extern int SetupPolygonAccessFromShapeIndex(int shapeIndex);
