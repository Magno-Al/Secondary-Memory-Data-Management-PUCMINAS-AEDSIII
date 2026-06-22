#ifndef MODULE_DAO_CPP
#define MODULE_DAO_CPP

#include "../models/Module.h"
#include "../utils/CrudTemplate.h"

class ModuleDAO : public CrudTemplate<Module>
{
    public:
        ModuleDAO() : CrudTemplate<Module>("data/module.dat") {}
};

#endif // MODULE_DAO_CPP