#include "../models/Module.h"
#include "../utils/CrudTemplate.h"

class ModuleDAO : public CrudTemplate<Module> 
{
    public:
        ModuleDAO() : CrudTemplate<Module>("../data/module.dat") {}
};