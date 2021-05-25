/*
from MethodAccess call, Containment c, Ref r
where (d.getL() = call.getCaller() and d.getR() == call.getCallee) ||
select
*/

/* exist call, c satisfy */
/*
from MethodAccess mcall, Changed c
where mcall.getCallee+() = c.getFQN()
select mcall
*/

/*
class MethodAccess {
}

class Containment {
}

class Ref {
}

class DepnedOn extends MethodAccess, Containment, Ref {
}

*/


import java

from Clazz cl, Insert ins, Update upd, MethodAccess ma, Method m, Containment ct, Clazz clsub
range upd @ (v1..v2), ins @ (v1..v2), ma @ v2, m @ v2, cl @ v2, clsub @ v2
where (upd = ma.getEe+() or ins = ma.getEe+()) and ma.getEr() = m
and ct.getEe+() = m and ct.getEr() = cl
and cl.isTestClass()
// and (!cl.isAbstractClass() or (clsub = cl.isAbstractClass().getSubclasses() and !clsub.isAbstractClass()))
select cl.getOuterClass()
