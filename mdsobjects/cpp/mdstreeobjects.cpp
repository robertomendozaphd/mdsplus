#include "mdsobjects.h"
#include <usagedef.h>

//#include "mdstree.h"
using namespace MDSobjects;

#ifdef HAVE_WINDOWS_H
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#define MAX_ARGS 512

extern "C" {
	void *convertToScalarDsc(int clazz, int dtype, int length, char *ptr);
	void *evaluateData(void *dscPtr, int isEvaluate);
	void freeDsc(void *dscPtr);
	void *convertFromDsc(void *dscPtr);
	char *decompileDsc(void *dscPtr);
	char *decompileDsc(void *dscPtr);
	void *compileFromExprWithArgs(char *expr, int nArgs, void *args, void *tree);
	void freeChar(void *);
	void *convertToArrayDsc(int clazz, int dtype, int length, int l_length, int nDims, int *dims, void *ptr);
	void *convertToCompoundDsc(int clazz, int dtype, int length, void *ptr, int ndescs, void **descs);
	void *convertToApdDsc(int ndescs, void **ptr);
	void *deserializeData(char *serialized, int size);

	void * convertToByte(void *dsc); 
	void * convertToShort(void *dsc); 
	void * convertToInt(void *dsc); 
	void * convertToLong(void *dsc); 
	void * convertToFloat(void *dsc); 
	void * convertToDouble(void *dsc); 
	void * convertToShape(void *dcs);


}




extern "C" void RTreeSynch();
extern "C" int _TreeOpen(void *dbid, char *tree, int shot, int readOnly);
extern "C" int _TreeClose(void *dbid, char *tree, int shot);
extern "C" int _RTreeOpen(void *dbid, char *tree, int shot);
extern "C" int _RTreeClose(void *dbid, char *tree, int shot);
extern "C" char *_RTreeSetCallback(void *dbid, int nid, void *argument, void (*callback)(int, void *));
extern "C" char _RTreeSetWarm(void *dbid, int nid, int warm);
extern "C" int _RTreeClearCallback(void *dbid, int nid, char *callbackDescr);
extern "C" int _TreeClose(void *dbid, char *tree, int shot);
extern "C" int _TreeFindNode(void *dbid, char *path, int *nid);
extern "C" int _TreeFindNodeWild(void *dbid, char *path, int *nid, void **ctx, int mask);
extern "C" int _TreeFindNodeEnd(void *dbid, void **ctx);
extern "C" char *_TreeGetPath(void *dbid, int nid);
extern "C" void TreeFree(void *ptr);
extern "C" int _TreeGetDbi(void *dbid, struct dbi_itm *itmlst);
extern "C" int TreeGetDbi(struct dbi_itm *itmlst);
extern "C" int _TreeGetNci(void *dbid, int nid, struct nci_itm *itmlst);
extern "C" int _TreeSetNci(void *dbid, int nid, struct nci_itm *itmlst);
extern "C" int _TreeIsOn(void *dbid, int nid);
extern "C" int _TreeTurnOff(void *dbid, int nid);
extern "C" int _TreeTurnOn(void *dbid, int nid);
extern "C" void convertTime(void *dbid, int *time, char *retTime);
extern "C" int _TreeSetDefaultNid(void *dbid, int nid);
extern "C" int _TreeGetDefaultNid(void *dbid, int *nid);
extern "C" char *_TreeFindNodeTags(void *dbid, int nid, void *ctx);
extern "C" int TreeFindTagEnd(void *ctx);
extern "C" void *TreeSwitchDbid(void *dbid);
extern "C" void RTreeConfigure(int shared, int size);

extern "C" int getTreeData(void *dbid, int nid, void **data, int isCached);
extern "C" int putTreeData(void *dbid, int nid, void *data, int isCached, int cachePolicy);
extern "C" int deleteTreeData(void *dbid, int nid, int isCached, int cachePolicy);
extern "C" int doTreeMethod(void *dbid, int nid, char *method);
extern "C" int beginTreeSegment(void *dbid, int nid, void *dataDsc, void *startDsc, void *endDsc, 
								void *timeDsc, int isCached, int cachePolicy);
extern "C" int putTreeSegment(void *dbid, int nid, void *dataDsc, int ofs, int isCached, int cachePolicy);
extern "C" int updateTreeSegment(void *dbid, int nid, void *startDsc, void *endDsc, 
								void *timeDsc, int isCached, int cachePolicy);
extern "C" int getTreeNumSegments(void *dbid, int nid, int *numSegments, int isCached); 
extern "C" int getTreeSegmentLimits(void *dbid, int nid, void **startDsc, void **endDsc, int isCached);
extern "C" int getTreeSegment(void *dbid, int nid, int segIdx, void **dataDsc, int isCached);
extern "C" int setTreeTimeContext(void *startDsc, void *endDsc, void *deltaDsc);//No cache option  
extern "C" int beginTreeTimestampedSegment(void *dbid, int nid, void *dataDsc, int isCached, int cachePolicy);
extern "C" int putTreeTimestampedSegment(void *dbid, int nid, void *dataDsc, _int64 *times, int isCached, int cachePolicy);
extern "C" int putTreeRow(void *dbid, int nid, void *dataDsc, _int64 *time, int isCached, int isLast, int cachePolicy);
extern "C" int LibConvertDateString(char *asc_time, _int64 *qtime);
extern "C" int TreeSetViewDate(_int64 *date);

extern "C" const char *MdsClassString(int id);
extern "C" const char *MdsDtypeString(int id);



extern "C" int _RTreeFlush(void *dbid, int nid);

#ifdef HAVE_WINDOWS_H
#include <Windows.h>
static HANDLE semH = 0;
void Tree::lock() 
{
	if(!semH)
		semH = CreateSemaphore(NULL, 1, 16, NULL);
	if(!semH)
		throw new TreeException("Cannot create lock semaphore");
    WaitForSingleObject(semH, INFINITE);      
}
void Tree::unlock()
{
	if(semH)
		ReleaseSemaphore(semH, 1, NULL);
}
#else
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <semaphore.h>
static 	sem_t semStruct;
static bool semInitialized = false;


void Tree::lock() 
{
	if(!semInitialized)
	{
		semInitialized = true;
		int status = sem_init(&semStruct, 0, 1);
		if(status != 0)
			throw new TreeException("Cannot create lock semaphore");
	}
	sem_wait(&semStruct);
}
void Tree::unlock()
{
	sem_post(&semStruct);
}
#endif


		
Tree::Tree(char *name, int shot)
{
	this->shot = shot;
	ctx = 0;
	int status = _TreeOpen(&ctx, name, shot, 0);
	if(!(status & 1))
	{
		throw new TreeException(status);
	}
	this->name = new char[strlen(name) + 1];
	strcpy(this->name, name);
}


Tree::~Tree()
{
    _TreeClose(ctx, name, shot);
    delete [] name;
}


TreeNode *Tree::getNode(char *path)
{
	int nid, status;

	status = _TreeFindNode(ctx, path, &nid);
	if(!(status & 1))
	{
		throw new TreeException(status);
	}
	return new TreeNode(nid, this);
}

	
TreeNode *Tree::getNode(TreePath *path)
{
	int nid, status;
	char *pathName = path->getString();
	status = _TreeFindNode(ctx, pathName, &nid);
	delete[] pathName;
	if(!(status & 1))
	{
		throw new TreeException(status);
	}
	return new TreeNode(nid, this);
}

	

TreeNodeArray *Tree::getNodeWild(char *path, int usageMask)
{
	int currNid, status; 
	int numNids = 0;
	void *wildCtx = 0;

	while ((status = _TreeFindNodeWild(ctx, path,&currNid,&wildCtx, usageMask)) & 1)
		numNids++;
	_TreeFindNodeEnd(ctx, &wildCtx);

	//printf("%s\n", MdsGetMsg(status));

	TreeNode **retNodes = new TreeNode *[numNids];
	ctx = 0;
	for(int i = 0; i < numNids; i++)
	{
		_TreeFindNodeWild(ctx, path,&currNid,&wildCtx, usageMask);
		retNodes[i] = new TreeNode(currNid, this);
	}
	_TreeFindNodeEnd(ctx, &wildCtx);
	TreeNodeArray *nodeArray = new TreeNodeArray(retNodes, numNids);
	delete [] retNodes;
	return nodeArray;
}

TreeNodeArray *Tree::getNodeWild(char *path)
{

	return getNodeWild(path, -1);
}


void Tree::setDefault(TreeNode *treeNode)
{
	int status = _TreeSetDefaultNid(ctx, treeNode->getNid());
	if(!(status & 1)) 
		throw new TreeException(status);
}

TreeNode *Tree::getDefault()
{
	int nid;

	int status = _TreeGetDefaultNid(ctx, &nid);
	if(!(status & 1)) 
		throw new TreeException(status);
	return new TreeNode(nid, this);
}

bool Tree::supportsVersions()
{
	int supports, len, status;
	struct dbi_itm dbiList[] = 
	{{sizeof(int), DbiVERSIONS_IN_PULSE, &supports, &len},
	{0, DbiEND_OF_LIST,0,0}};

	status = _TreeGetDbi(ctx, dbiList);
	if(!(status & 1)) 
		throw new TreeException(status);
	return (supports)?true:false;
}

void Tree::setViewDate(char *date)
{
	_int64  qtime;

	int status = LibConvertDateString(date, &qtime);
	if(!(status & 1))
		throw new TreeException("Invalid date format");
	status = TreeSetViewDate(&qtime);
	if(!(status & 1)) 
		throw new TreeException(status);
}


void Tree::setTimeContext(Data *start, Data *end, Data *delta)
{
	int status = setTreeTimeContext((start)?start->convertToDsc():0, (end)?end->convertToDsc():0, 
		(delta)?delta->convertToDsc():0);
	if(!(status & 1))
		throw new TreeException(status);
}


void *TreeNode::convertToDsc()
{
	Tree::lock();
	setActiveTree(tree);
	void *retDsc = completeConversionToDsc(convertToScalarDsc(clazz, dtype, sizeof(int), (char *)&nid));
	Tree::unlock();
	return retDsc;
}

int TreeNode::getFlag(int flagOfs)
{
	int nciFlags;
	int nciFlagsLen = sizeof(int);
	struct nci_itm nciList[] =  {{4, NciGET_FLAGS, &nciFlags, &nciFlagsLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);
	return (nciFlags & flagOfs)?true:false;
}

void TreeNode::setFlag(int flagOfs, bool val)
{
	int nciFlags;
	int nciFlagsLen = sizeof(int);
	struct nci_itm nciList[] =  {{4, NciGET_FLAGS, &nciFlags, &nciFlagsLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
	{
		Tree::unlock();
		throw new TreeException(status);
	}
	if(val)
		nciFlags |= flagOfs;
	else
		nciFlags &= ~flagOfs;

	status = _TreeSetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);
}

TreeNode::TreeNode(int nid, Tree *tree, Data *units, Data *error, Data *help, Data *validation)
{
	if(!tree)
		throw new DataException(CLASS_S, DTYPE_NID, "A Tree instance must be defined when ceating TreeNode instances");
	this->nid = nid;
	this->tree = tree;
	clazz = CLASS_S;
	dtype = DTYPE_NID;
	setAccessory(units, error, help, validation);
}

		
char *TreeNode::getPath()
{
	resolveNid();
	char *currPath = _TreeGetPath(tree->getCtx(), nid);
	char *path = new char[strlen(currPath) + 1];
	strcpy(path, currPath);
	TreeFree(currPath);
	return path;
}


char *TreeNode::getMinPath()
{
	char path[1024];
	int pathLen = 1024;
	struct nci_itm nciList[] = 
		{{511, NciMINPATH, path, &pathLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);
	char *retPath = new char[strlen(path)+1];
	strcpy(retPath, path);
	return retPath;
}

char *TreeNode::getFullPath()
{
	char path[1024];
	int pathLen = 1024;
	struct nci_itm nciList[] = 
		{{511, NciFULLPATH, path, &pathLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);
	char *retPath = new char[strlen(path)+1];
	strcpy(retPath, path);
	return retPath;
}

char *TreeNode::getOriginalPartName()
{
	char path[1024];
	int pathLen = 1024;
	struct nci_itm nciList[] = 
		{{511, NciORIGINAL_PART_NAME, path, &pathLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);
	char *retPath = new char[strlen(path)+1];
	strcpy(retPath, path);
	return retPath;
}

char *TreeNode::getNodeName()
{
	char path[1024];
	int pathLen = 1024;
	struct nci_itm nciList[] = 
		{{511, NciNODE_NAME, path, &pathLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);
	char *retPath = new char[strlen(path)+1];
	strcpy(retPath, path);
	return retPath;
}


Data *TreeNode::getData()
{
	Data *data = 0;
	resolveNid();
	int status = getTreeData(tree->getCtx(), nid, (void **)&data, isCached());
	if(!(status & 1))
	{
		throw new TreeException(status);
	}
	return data;
}

void TreeNode::putData(Data *data)
{
	resolveNid();
	int status = putTreeData(tree->getCtx(), nid, (void *)data, isCached(), getCachePolicy());
	if(!(status & 1))
	{
		throw new TreeException(status);
	}
}
void TreeNode::deleteData()
{
	resolveNid();
	int status = deleteTreeData(tree->getCtx(), nid, isCached(), getCachePolicy());
	if(!(status & 1))
	{
		throw new TreeException(status);
	}
}



bool TreeNode::isOn()
{
	resolveNid();
	bool retOn = _TreeIsOn(tree->getCtx(), nid)?true:false;
	return  retOn;
}
void TreeNode::setOn(bool on)
{
	resolveNid();
	if(on)
		_TreeTurnOn(tree->getCtx(), nid);
	else
		_TreeTurnOff(tree->getCtx(), nid);
}

int TreeNode::getLength()
{
	int length;
	int lengthLen = sizeof(int);
	struct nci_itm nciList[] = 
		{{4, NciLENGTH, &length, &lengthLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);
	return length;
}
int TreeNode::getCompressedLength()
{
	int length;
	int lengthLen = sizeof(int);
	struct nci_itm nciList[] = 
		{{4, NciRLENGTH, &length, &lengthLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);
	return length;
}

int TreeNode::getStatus()
{
	int id;
	int idLen = sizeof(int);
	struct nci_itm nciList[] = 
		{{4, NciSTATUS, &id, &idLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);
	return id;
}

int TreeNode::getOwnerId()
{
	int id;
	int idLen = sizeof(int);
	struct nci_itm nciList[] = 
		{{4, NciOWNER_ID, &id, &idLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);
	return id;
}

_int64 TreeNode::getTimeInserted()
{
	_int64 timeInserted; 
	int timeLen;
	struct nci_itm nciList[] = 
		{{8, NciTIME_INSERTED, (char *)&timeInserted, &timeLen},
		{NciEND_OF_LIST, 0, 0, 0}};
	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);
	return timeInserted;
}

void TreeNode::doMethod(char *method)
{
	resolveNid();
	int status = doTreeMethod(tree->getCtx(), nid, method);
	if(!(status & 1))
		throw new TreeException(status);
}

bool TreeNode::isSetup()
{
	return getFlag(NciM_SETUP_INFORMATION)?true:false;
}


bool TreeNode::isWriteOnce()
{
	return getFlag(NciM_WRITE_ONCE)?true:false;
}

void TreeNode::setWriteOnce(bool flag)
{
	setFlag(NciM_WRITE_ONCE, flag);
}

bool TreeNode::isCompressOnPut()
{
	return getFlag(NciM_COMPRESS_ON_PUT)?true:false;
}

void TreeNode::setCompressOnPut(bool flag)
{
	setFlag(NciM_COMPRESS_ON_PUT, flag);
}
bool TreeNode::isNoWriteModel()
{
	return getFlag(NciM_NO_WRITE_MODEL)?true:false;
}

void TreeNode::setNoWriteModel(bool flag)
{
	setFlag(NciM_NO_WRITE_MODEL, flag);
}

bool TreeNode::isEssential()
{
	return getFlag(NciM_ESSENTIAL)?true:false;
}

void TreeNode::setEssential(bool flag)
{
	setFlag(NciM_ESSENTIAL, flag);
}


bool TreeNode::isNoWriteShot()
{
	return getFlag(NciM_NO_WRITE_SHOT)?true:false;
}

void TreeNode::setNoWriteShot(bool flag)
{
	setFlag(NciM_NO_WRITE_SHOT, flag);
}

bool TreeNode::isIncludedInPulse()
{
	return getFlag(NciM_INCLUDE_IN_PULSE)?true:false;
}
bool TreeNode::containsVersions()
{
	return getFlag(NciM_VERSIONS)?true:false;
}
bool TreeNode::isMember()
{
	int parLen = 4;
	int par;
	struct nci_itm nciList[] = 
		{{4, NciPARENT_RELATIONSHIP, &par, &parLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);
	
	return  (par & NciK_IS_MEMBER)?true:false;
}

bool TreeNode::isChild()
{
	int parLen = 4;
	int par;
	struct nci_itm nciList[] = 
		{{4, NciPARENT_RELATIONSHIP, &par, &parLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);
	
	return  (par & NciK_IS_MEMBER)?true:false;
}

void TreeNode::setIncludedInPulse(bool flag)
{
	setFlag(NciM_INCLUDE_IN_PULSE, flag);
}

TreeNode *TreeNode::getParent()
{

	int nidLen = 4;
	int parentNid;
	struct nci_itm nciList[] = 
		{{4, NciPARENT, &parentNid, &nidLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);

	return new TreeNode(parentNid, tree);
}

TreeNode *TreeNode::getBrother()
{

	int nidLen = 4;
	int brotherNid;
	struct nci_itm nciList[] = 
		{{4, NciBROTHER, &brotherNid, &nidLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);

	return new TreeNode(brotherNid, tree);
}

TreeNode *TreeNode::getChild()
{

	int nidLen = 4;
	int childNid;
	struct nci_itm nciList[] = 
		{{4, NciCHILD, &childNid, &nidLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);

	return new TreeNode(childNid, tree);
}

TreeNode *TreeNode::getMember()
{

	int nidLen = 4;
	int memberNid;
	struct nci_itm nciList[] = 
		{{4, NciCHILD, &memberNid, &nidLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);

	return new TreeNode(memberNid, tree);
}


int TreeNode::getNumChildren()
{
	int numLen = 4;
	int nChildren;
	struct nci_itm nciList[] = 
	{{4, NciNUMBER_OF_CHILDREN, &nChildren, &numLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);
	return nChildren;
}

int TreeNode::getNumMembers()
{
	int numLen = 4;
	int nChildren;
	struct nci_itm nciList[] = 
	{{4, NciNUMBER_OF_MEMBERS, &nChildren, &numLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);
	return nChildren;
}

int TreeNode::getNumDescendants()
{
	int numLen = 4;
	int numMembers, nChildren;
	struct nci_itm nciList[] = 
		{{4, NciNUMBER_OF_MEMBERS, &numMembers, &numLen},
		{4, NciNUMBER_OF_CHILDREN, &nChildren, &numLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);

	return numMembers+nChildren;
}


TreeNode **TreeNode::getDescendants(int *numDescendants)
{

	int numLen = 4;
	int numMembers, nChildren;
	struct nci_itm nciList[] = 
		{{4, NciNUMBER_OF_MEMBERS, &numMembers, &numLen},
		{4, NciNUMBER_OF_CHILDREN, &nChildren, &numLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
	{
		throw new TreeException(status);
	}

	int *childrenNids = new int[numMembers+nChildren];

	int retLen = sizeof(int) * (numMembers+nChildren);
	struct nci_itm nciList1[] = 
		{{4, NciMEMBER_NIDS, &childrenNids[0], &retLen},
		{4, NciCHILDREN_NIDS, &childrenNids[numMembers], &retLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
	{
		delete [] childrenNids;
		throw new TreeException(status);
	}
	TreeNode **retChildren = new TreeNode* [numMembers+nChildren];
	for(int i = 0; i < numMembers+nChildren; i++)
		retChildren[i] = new TreeNode(childrenNids[i], tree);

	delete[] childrenNids;
	*numDescendants = numMembers+nChildren;
	return retChildren;
}

TreeNode **TreeNode::getChildren(int *numChildren)
{

	int numLen = 4;
	int nChildren;
	struct nci_itm nciList[] = 
	{{4, NciNUMBER_OF_CHILDREN, &nChildren, &numLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
	{
		throw new TreeException(status);
	}

	int *childrenNids = new int[nChildren];

	int retLen = sizeof(int) * (nChildren);
	struct nci_itm nciList1[] = 
		{{4, NciCHILDREN_NIDS, &childrenNids[0], &retLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
	{
		delete [] childrenNids;
		throw new TreeException(status);
	}
	TreeNode **retChildren = new TreeNode* [nChildren];
	for(int i = 0; i < nChildren; i++)
		retChildren[i] = new TreeNode(childrenNids[i], tree);

	delete[] childrenNids;
	*numChildren = nChildren;
	return retChildren;
}

TreeNode **TreeNode::getMembers(int *numMembers)
{

	int numLen = 4;
	int nMembers;
	struct nci_itm nciList[] = 
	{{4, NciNUMBER_OF_MEMBERS, &nMembers, &numLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
	{
		throw new TreeException(status);
	}

	int *memberNids = new int[nMembers];

	int retLen = sizeof(int) * (nMembers);
	struct nci_itm nciList1[] = 
		{{4, NciMEMBER_NIDS, &memberNids[0], &retLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
	{
		delete [] memberNids;
		throw new TreeException(status);
	}
	TreeNode **retMembers = new TreeNode* [nMembers];
	for(int i = 0; i < nMembers; i++)
		retMembers[i] = new TreeNode(memberNids[i], tree);

	delete[] memberNids;
	*numMembers = nMembers;
	return retMembers;
}



const char *TreeNode::getClass()
{

	int clsLen = 1;
	char cls;
	struct nci_itm nciList[] = 
		{{1, NciCLASS, &cls, &clsLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);

	return MdsClassString(cls);
}
	
const char *TreeNode::getDType()
{

	int clsLen = 1;
	char cls;
	struct nci_itm nciList[] = 
		{{1, NciDTYPE, &cls, &clsLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);

	return MdsDtypeString(cls);
}
	
const char *TreeNode::getUsage()
{

	int usageLen = 4;
	int usage;
	struct nci_itm nciList[] = 
		{{4, NciUSAGE, &usage, &usageLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);

	switch(usage)  {
		case TreeUSAGE_ACTION: return "ACTION";
		case TreeUSAGE_ANY: return "ANY";
		case TreeUSAGE_AXIS: return "AXIS";
		case TreeUSAGE_COMPOUND_DATA: return "COMPOUND_DATA";
		case TreeUSAGE_DEVICE: return "DEVICE";
		case TreeUSAGE_DISPATCH: return "DISPATCH";
		case TreeUSAGE_STRUCTURE: return "STRUCTURE";
		case TreeUSAGE_NUMERIC: return "NUMERIC";
		case TreeUSAGE_SIGNAL: return "SIGNAL";
		case TreeUSAGE_SUBTREE: return "SUBTREE";
		case TreeUSAGE_TASK: return "TASK";
		case TreeUSAGE_TEXT: return "TEXT";
		case TreeUSAGE_WINDOW: return "WINDOW";
	}
	return "Unknown";
}

int TreeNode::getConglomerateElt()
{

	int eltLen;
	int elt;
	struct nci_itm nciList[] = 
		{{1, NciCONGLOMERATE_ELT, (char *)&elt, &eltLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);

	return elt;
}
int TreeNode::getNumElts()
{

	int nNidsLen;
	int nNids;
	struct nci_itm nciList[] = 
		{{4, NciNUMBER_OF_ELTS, (char *)&nNids, &nNidsLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);
	return nNidsLen;
}

	
TreeNodeArray *TreeNode::getConglomerateNids()
{

	int nNidsLen, retLen;
	int nNids;
	struct nci_itm nciList[] = 
		{{4, NciNUMBER_OF_ELTS, (char *)&nNids, &nNidsLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
	{
		throw new TreeException(status);
	}

	int *nids = new int[nNids];
	struct nci_itm nciList1[] = 
		{{4*nNids, NciCONGLOMERATE_ELT, (char *)nids, &retLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	status = _TreeGetNci(tree->getCtx(), nid, nciList1);
	if(!(status & 1))
		throw new TreeException(status);

	TreeNodeArray *resArray = new TreeNodeArray(nids, nNids, tree);
	delete [] nids;
	return resArray;
}
	
int TreeNode::getDepth()
{

	int depthLen;
	int depth;
	struct nci_itm nciList[] = 
		{{1, NciDEPTH, (char *)&depth, &depthLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);

	return depth;
}
	

char **TreeNode::getTags(int *numRetTags)
{
	const int MAX_TAGS = 128;
	
	char *tags[MAX_TAGS];
	int numTags;
	void *ctx = 0;
	resolveNid();
	for(numTags = 0; numTags < MAX_TAGS; numTags++)
	{
		tags[numTags] = _TreeFindNodeTags(tree->getCtx(), nid, &ctx);
		if(!tags[numTags]) break;
	}
	TreeFindTagEnd(&ctx);
	*numRetTags = numTags;
	char **retTags = new char *[numTags];
	for(int i = 0; i < numTags; i++)
	{
		retTags[i] = new char(strlen(tags[i])+1);
		strcpy(retTags[i], tags[i]);
		TreeFree(tags[i]);
	}
	return retTags;
}

void TreeNode::beginSegment(Data *start, Data *end, Data *time, Array *initialData)
{
	resolveNid();
	int status = beginTreeSegment(tree->getCtx(), getNid(), initialData->convertToDsc(), start->convertToDsc(), 
		end->convertToDsc(), time->convertToDsc(), isCached(), getCachePolicy());
	if(!(status & 1))
		throw new TreeException(status);
}

void TreeNode::putSegment(Array *data, int ofs)
{
	resolveNid();
	int status = putTreeSegment(tree->getCtx(), getNid(), data->convertToDsc(), ofs, isCached(), getCachePolicy());
	if(!(status & 1))
		throw new TreeException(status);
}

void TreeNode::updateSegment(Data *start, Data *end, Data *time)
{
	resolveNid();
	int status = updateTreeSegment(tree->getCtx(), getNid(), start->convertToDsc(), 
		end->convertToDsc(), time->convertToDsc(), isCached(), getCachePolicy());
	if(!(status & 1))
		throw new TreeException(status);
}

int TreeNode::getNumSegments()
{
	int numSegments;

	resolveNid();
	int status = getTreeNumSegments(tree->getCtx(), getNid(), &numSegments, isCached());
	if(!(status & 1))
		throw new TreeException(status);
	return  numSegments;
}

void TreeNode::getSegmentLimits(int segmentIdx, Data **start, Data **end)
{
	void *startDsc, *endDsc;
	resolveNid();
	int status = getTreeSegmentLimits(tree->getCtx(), getNid(), &startDsc, &endDsc, isCached());
	if(!(status & 1))
		throw new TreeException(status);
	*start = (Data*)convertFromDsc(startDsc);
	freeDsc(startDsc);
	*end = (Data*)convertFromDsc(endDsc);
	freeDsc(endDsc);
}
	
Array *TreeNode::getSegment(int segIdx)
{
	void *dataDsc;
	
	resolveNid();
	int status = getTreeSegment(tree->getCtx(), getNid(), segIdx, &dataDsc, isCached());
	if(!(status & 1))
		throw new TreeException(status);
	Array *retData = (Array *)convertFromDsc(dataDsc);
	freeDsc(dataDsc);
	return retData;
}


void TreeNode::beginTimestampedSegment(Array *initData)
{
	resolveNid();
	int status = beginTreeTimestampedSegment(tree->getCtx(), getNid(), initData->convertToDsc(), isCached(), getCachePolicy());
	if(!(status & 1))
		throw new TreeException(status);
}

void TreeNode::putTimestampedSegment(Array *data, Int64Array *times)
{
	int nTimesArray;
	_int64 *timesArray = times->getLongArray(&nTimesArray);
	resolveNid();
	int status = putTreeTimestampedSegment(tree->getCtx(), getNid(), data->convertToDsc(), timesArray, isCached(), getCachePolicy());
	delete [] timesArray;
	if(!(status & 1))
		throw new TreeException(status);
}

void TreeNode::putRow(Data *data, Int64 *time)
{
	_int64 time64 = time->getLong();
	resolveNid();
	int status = putTreeRow(tree->getCtx(), getNid(), data->convertToDsc(), &time64, isCached(), false, getCachePolicy());
	if(!(status & 1))
		throw new TreeException(status);
}

void TreeNode::acceptSegment(Array *data, Data *start, Data *end, Data *times)
{
	resolveNid();
	int status = beginTreeSegment(tree->getCtx(), getNid(), data->convertToDsc(), start->convertToDsc(), 
		end->convertToDsc(), times->convertToDsc(), isCached(), getCachePolicy());
	if(!(status & 1))
		throw new TreeException(status);
}
void TreeNode::acceptRow(Data *data, _int64 time, bool isLast)
{
	resolveNid();
	int status = putTreeRow(tree->getCtx(), getNid(), data->convertToDsc(), &time, isCached(), false, getCachePolicy());
	if(!(status & 1))
		throw new TreeException(status);
}


TreePath::TreePath(char *val, Tree *tree, Data *units, Data *error, Data *help, Data *validation):TreeNode(0, tree, units, error, help,validation)
{
	clazz = CLASS_S;
	dtype = DTYPE_PATH;
	length = strlen(val);
	ptr = new char[length];
	memcpy(ptr, val, length);
	setAccessory(units, error, help, validation);
}
TreePath::TreePath(char *val, int len, Tree *tree, Data *units, Data *error, Data *help, Data *validation):TreeNode(0, tree, units, error, help,validation)
{
	clazz = CLASS_S;
	dtype = DTYPE_PATH;
	length = len;
	ptr = new char[length];
	memcpy(ptr, val, length);
	setAccessory(units, error, help, validation);
}

void TreePath::resolveNid()
{
	int status = _TreeFindNode(tree->getCtx(), ptr, &nid);
	if(!(status & 1))
		nid = -1;
}
	
void CachedTreeNode::flush() {_RTreeFlush(tree->getCtx(), getNid());}

void CachedTree::open() 
{
	int status;
	
	RTreeConfigure(1, cacheSize);
	status = _RTreeOpen(getCtx(), name, shot);
	if(!(status & 1))
	{
		throw new TreeException(status);
	}
}

void CachedTree::close()
{
	int status = _RTreeClose(getCtx(), name, shot);
	if(!(status & 1))
	{
		throw new TreeException(status);
	}
}

CachedTreeNode *CachedTree::getCachedNode(char *path)
{
	int nid, status;

	status = _TreeFindNode(getCtx(), path, &nid);
	if(!(status & 1))
	{
		throw new TreeException(status);
	}
	return new CachedTreeNode(nid, this);
}


void CachedTreeNode::putLastRow(Data *data, Int64 *time)
{
	_int64 time64 = time->getLong();
	resolveNid();
	int status = putTreeRow(tree->getCtx(), getNid(), data->convertToDsc(), &time64, true, true, getCachePolicy());
	if(!(status & 1))
		throw new TreeException(status);
}


void CachedTree::synch()
{
	RTreeSynch();
}

void MDSobjects::setActiveTree(Tree *tree)
{
	TreeSwitchDbid(tree->getCtx());
}

Tree *MDSobjects::getActiveTree()
{
	char name[1024];
	int shot;
	int retNameLen, retShotLen;
	
	DBI_ITM dbiItems[] = {
		{1024, DbiNAME, name, &retNameLen},
		{sizeof(int), DbiSHOTID, &shot, &retShotLen},
		{0, DbiEND_OF_LIST, 0, 0}};
	int 
		status = TreeGetDbi(dbiItems);
	if(!(status & 1))
		throw new TreeException(status);
	return new Tree(name, shot);
}

ostream &operator<<(ostream &stream, TreeNode *treeNode)
{
	return stream << treeNode->getPath();
}
