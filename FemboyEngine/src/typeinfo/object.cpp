#include "Object.h"

namespace fe {

typeinfo::detail::SetupHelper<void, Object> Object::typeInfoSetupHelper;

const TypeInfo* Object::GetTypeInfo() const
{
	return typeInfoSetupHelper.getTypeInfo();
}

}