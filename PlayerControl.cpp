#include "PlayerControl.h"

#include "ScriptFactory.h"

#include "CSVExporter.h"

REGISTER_SCRIPT(PlayerControl)



void PlayerControl::Start()
{
}

void PlayerControl::Update()
{
}

void PlayerControl::Export() {
    CSVExporter::ExportFloat(m_Speed);
}
