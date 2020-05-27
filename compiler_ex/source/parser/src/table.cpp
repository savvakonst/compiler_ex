
#include "table.h"
#include "variable.h"


void Block::setUint(Variable * var)
{
    unitList.push(var);

}

void TableColumn::setUint(Variable * var)
{
    auto  varlevel= var->getLevel();
    for (auto i : blockList) 
        if (i->getLevel() == varlevel) {
            i->setUint(var);
            return;
        }
    blockList.push(new Block(var));
}

void Table::setUint(Variable * var)
{
    auto  varLength= var->getLength();
    for (auto i : columnList)
        if (i->getLength() == varLength) {
            i->setUint(var);
            return;
        }
    columnList.push();
}