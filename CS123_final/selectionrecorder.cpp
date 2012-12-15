#include "selectionrecorder.h"
#include <qgl.h>
#include <assert.h>
#include <iostream>

// from http://en.wikipedia.org/wiki/Assertion_(computing)
#define COMPILE_TIME_ASSERT(pred) switch(0){case 0:case pred:;}

using namespace std;

SelectionRecorder::SelectionRecorder()
{
    m_selectionBuffer = NULL;
}

SelectionRecorder::~SelectionRecorder()
{
    delete [] m_selectionBuffer;
}

void SelectionRecorder::enterSelectionMode(int x, int y, int numObjects)
{
    if(m_selectionBuffer != NULL)
    {
        cout << "error: enterSelectionMode() called twice without calling exitSelectionMode() in between" << endl;
        assert(false);
        return;
    }

    // Get information about the current OpenGL state.
    int viewport[4];
    double projectionMatrix[16];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);

    // Make the origin in the upper left.
    y = viewport[3] - y - 1;

    // Pre-multiply the projection matrix with a pick matrix, which expands
    // the viewport to fill the 1x1 square under the mouse pointer at (x, y).
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPickMatrix((double)x, (double)y, 1, 1, viewport);
    glMultMatrixd(projectionMatrix);
    glMatrixMode(GL_MODELVIEW);

    // Make sure the members of HitRecord are packed tightly.
    COMPILE_TIME_ASSERT(sizeof(HitRecord) == sizeof(unsigned int) * 4);

    // Enter selection mode with enough room for numObjects hit records.
    m_selectionBuffer = new HitRecord[numObjects < 1 ? 1 : numObjects];
    glSelectBuffer(numObjects * 4, (unsigned int *)m_selectionBuffer);
    glRenderMode(GL_SELECT);
    glInitNames();
    glPushName(0);

    // Remember the mouse position for reconstructing the hit point later.
    m_mouseX = x;
    m_mouseY = y;
}

void SelectionRecorder::setObjectIndex(int index)
{
   // cout << "selectionRecord:setObjectIn " << index << endl;
    // Replace the current name on top of the name stack with index.
    // We only use a name stack of depth one, so it's like a variable.
    glLoadName(index);
}

bool SelectionRecorder::exitSelectionMode(int &index, double &hitX, double &hitY, double &hitZ)
{
    // Put the user's projection matrix back the way it was.
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    // Leave selection mode and parse the hits it generated.
    bool hasSelection = false;
    int hits = glRenderMode(GL_RENDER);
   // cout <<"selectionrecord:exitselectionmode " << hits << endl;
    unsigned int minDepth = ~0;
    for (int i = 0; i < hits; i++)
    {
        const HitRecord &hit = m_selectionBuffer[i];
     //  cout << hit.name << endl;
        if (hit.minDepth < minDepth)
        {
            index = hit.name;
            minDepth = hit.minDepth;
            hasSelection = true;
        }
    }

    // We're done with the selection buffer.
    delete [] m_selectionBuffer;
    m_selectionBuffer = NULL;

    if (hasSelection)
    {
        int viewport[4];
        double x, y, z, modelviewMatrix[16], projectionMatrix[16];
        glGetIntegerv(GL_VIEWPORT, viewport);
        glGetDoublev(GL_MODELVIEW_MATRIX, modelviewMatrix);
        glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
        gluUnProject(m_mouseX, m_mouseY, minDepth / (double)0xFFFFFFFF,
                     modelviewMatrix, projectionMatrix, viewport, &x, &y, &z);
        hitX = x, hitY = y, hitZ = z;
        return true;
    }

    return false;
}
