if '__package__' not in globals() or __package__ is None or len(__package__)==0:
  def _mimport(name):
    return __import__(name,globals())
else:
  def _mimport(name):
    return __import__(name,globals(),{},[],1)

_data=_mimport('mdsdata')
_ver=_mimport('version')

class Ident(_data.Data):
    """Reference to MDSplus Ken Variable"""
    def __init__(self,name):
        self.name=_ver.tostr(name)
    def __str__(self):
        return self.name
