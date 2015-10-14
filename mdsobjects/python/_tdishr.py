if '__package__' not in globals() or __package__ is None or len(__package__)==0:
  def _mimport(name):
    return __import__(name,globals())
else:
  def _mimport(name):
    return __import__(name,globals(),{},[],1)

import ctypes as _C

_descriptor = _mimport('_descriptor')
_mdsshr=_mimport('_mdsshr')
_Exceptions=_mimport('mdsExceptions')
_tree=_mimport('tree')
_ver=_mimport('version')

TdiShr=_ver.load_library('TdiShr')

def _TdiShrFun(function,errormessage,expression,args=None):
    descriptor = _descriptor.descriptor
    def parseArguments(args):
        if args is None:
            return []
        if isinstance(args,tuple):
            if args:
                if isinstance(args[0],tuple):
                    return(parseArguments(args[0]))
            return([_C.pointer(descriptor(arg)) for arg in args])
        raise TypeError('Arguments must be passed as a tuple')
    xd = _descriptor.descriptor_xd()
    arguments = [_C.pointer(descriptor(expression))]+parseArguments(args)+[_C.pointer(xd),_C.c_void_p(-1)]
    Tree = _tree.Tree
    Tree.lock()
    try:
        tree = Tree.getActiveTree()
        if tree is not None:
            tree.restoreContext()
        status = function(*arguments)
    finally:
        Tree.unlock()
    if (status & 1 != 0):
        return xd.value
    else:
        raise _Exceptions.statusToException(status)

def TdiCompile(expression,args=None):
    """Compile a TDI expression. Format: TdiCompile('expression-string')"""
    return _TdiShrFun(TdiShr.TdiCompile,"Error compiling",expression,args)

def TdiExecute(expression,args=None):
    """Compile and execute a TDI expression. Format: TdiExecute('expression-string')"""
    return _TdiShrFun(TdiShr.TdiExecute,"Error executing",expression,args)

def TdiDecompile(expression):
    """Decompile a TDI expression. Format: TdiDecompile(tdi_expression)"""
    return _ver.tostr(_TdiShrFun(TdiShr.TdiDecompile,"Error decompiling",expression))

def TdiEvaluate(expression):
    """Evaluate and functions. Format: TdiEvaluate(data)"""
    return _TdiShrFun(TdiShr.TdiEvaluate,"Error evaluating",expression)

def TdiData(expression):
    """Return primiitive data type. Format: TdiData(value)"""
    return _TdiShrFun(TdiShr.TdiData,"Error converting to data",expression)

CvtConvertFloat=TdiShr.CvtConvertFloat
