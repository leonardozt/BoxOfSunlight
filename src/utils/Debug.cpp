#include "Debug.h"

namespace BOSL
{
	namespace debug
	{
        void GLAPIENTRY debugMessageCallback(
            GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar* message,
            const void* userParam)
        {
            fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
                (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
                type, severity, message);
            if (severity == GL_DEBUG_SEVERITY_HIGH) {
                throw BoxOfSunlightError("Error due to high debug severity");
            }
        }

        void printComputeLimits()
        {
            GLint workGroupCount[3];
            glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workGroupCount[0]);
            glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workGroupCount[1]);
            glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workGroupCount[2]);
            printf("Max work group counts\n x:%i y:%i z:%i\n",
                workGroupCount[0], workGroupCount[1], workGroupCount[2]);

            GLint workGroupSize[3];
            glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workGroupSize[0]);
            glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workGroupSize[1]);
            glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workGroupSize[2]);
            printf("Local size limitations\n x:%i y:%i z:%i\n",
                workGroupSize[0], workGroupSize[1], workGroupSize[2]);

            GLint workGroupInv;
            glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &workGroupInv);
            printf("Max number of invocations within work group : %i\n", workGroupInv);
        }
	}
}