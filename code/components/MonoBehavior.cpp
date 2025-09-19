#include "MonoBehavior.h"

void MonoBehaviour::UpdateComponent()
{
    if (!isStarted) {
        Start();
        isStarted = true; // Set the flag to true after Start is called
    }
    Update();
}
