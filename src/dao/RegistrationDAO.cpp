#include <iostream>
#include <vector>
#include <fstream>

#include "../models/Registration.h"
#include "../utils/CrudTemplate.h"
#include "../utils/ExtendableHash.h"

class RegistrationDAO : public CrudTemplate<Registration> 
{
private:
    ExtendableHash index;

public:
    RegistrationDAO() : 
        CrudTemplate<Registration>("../data/registration.dat"),
        index("../data/index_dir.dat", "../data/index_buckets.dat") {}

    bool create(Registration &r)
    {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        int64_t offset = file.tellg();
        file.close();

        if (offset < (int64_t)sizeof(int))
            offset = sizeof(int);

        if (CrudTemplate<Registration>::create(r))
        {
            index.insert(r.module_id, offset);
            return true;
        }
        return false;
    }

    std::vector<Registration> findByModule(int moduleId)
    {
        std::vector<Registration> results;
        std::vector<int64_t> offsets = index.search(moduleId);

        if (offsets.empty())
            return results;

        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
            return results;

        for (size_t i = 0; i < offsets.size(); i++)
        {
            file.seekg(offsets[i]);
            Registration r;
            file.read(reinterpret_cast<char *>(&r), sizeof(Registration));

            if (!r.removed && r.module_id == moduleId)
            {
                results.push_back(r);
            }
        }

        file.close();
        return results;
    }
};