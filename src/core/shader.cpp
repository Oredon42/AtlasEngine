#include "core/shader.h"

Shader::Shader() :
    m_uiProgram(0),
    m_iActiveTexture(0),
    m_bInitialised(false),
    m_strVertexPath(""),
    m_strFragmentPath(""),
    m_strGeometryPath("")
{

}

Shader::Shader(const std::string &strVertexPath, const std::string &strFragmentPath, const std::string &strGeometryPath) :
    m_uiProgram(0),
    m_iActiveTexture(0),
    m_bInitialised(false),
    m_strVertexPath(""),
    m_strFragmentPath(""),
    m_strGeometryPath("")
{
    init(strVertexPath, strFragmentPath, strGeometryPath);
}

void Shader::init(const std::string &strVertexPath, const std::string &strFragmentPath, const std::string &strGeometryPath)
{
    m_strVertexPath = strVertexPath;
    m_strFragmentPath = strFragmentPath;
    m_strGeometryPath = strGeometryPath;

    reload();
}

void Shader::reload()
{
    std::string strVertexShaderSource,
                strFragmentShaderSource,
                strGeometryShaderSource;

    const bool &bSuccess = loadSourceFromFiles(strVertexShaderSource, strFragmentShaderSource, strGeometryShaderSource);

    if(bSuccess == false && m_bInitialised == true)
    {
        m_bInitialised = false;
    }
    else
    {
        bSuccess = compileSourceCode(strVertexShaderSource, strFragmentShaderSource, strGeometryShaderSource);

        if(bSuccess == false && m_bInitialised == true)
        {
            m_bInitialised = false;
        }
        else if(m_bInitialised == false)
        {
            m_bInitialised = true;
        }
    }
}

void Shader::use() const
{
    if(m_bInitialised == true)
    {
        glUseProgram(m_uiProgram);
    }
    else
    {
        std::cerr << "ERROR::SHADER::USE The shader has to be initialised before it can be used" << std::endl;
    }
}

void Shader::setFragmentDefinedEnabled(const std::string &strKey, const bool &bEnabled)
{
    if(m_mapFragmentDefines.find(strKey) != m_mapFragmentDefines.end())
    {
        m_mapFragmentDefines.at(strKey).bEnabled = bEnabled;
    }
    else
    {
        m_mapFragmentDefines.insert({"", bEnabled});
    }
}

void Shader::setVertexDefinedEnabled(const std::string &strKey, const bool &bEnabled)
{
    if(m_mapVertexDefines.find(strKey) != m_mapVertexDefines.end())
    {
        m_mapVertexDefines.at(strKey).bEnabled = bEnabled;
    }
    else
    {
        m_mapVertexDefines.insert({"", bEnabled});
    }
}

void Shader::setGeometryDefinedEnabled(const std::string &strKey, const bool &bEnabled)
{
    if(m_mapGeometryDefines.find(strKey) != m_mapGeometryDefines.end())
    {
        m_mapGeometryDefines.at(strKey).bEnabled = bEnabled;
    }
    else
    {
        m_mapGeometryDefines.insert({"", bEnabled});
    }
}

void Shader::setFragmentDefineText(const std::string &strKey, const std::string &strText)
{
    if(m_mapFragmentDefines.find(strKey) != m_mapFragmentDefines.end())
    {
        m_mapFragmentDefines.at(strKey).strText = strText;
    }
    else
    {
        m_mapFragmentDefines.insert({strText, true});
    }
}

void Shader::setVertexDefineText(const std::string &strKey, const std::string &strText)
{
    if(m_mapVertexDefines.find(strKey) != m_mapVertexDefines.end())
    {
        m_mapVertexDefines.at(strKey).strText = strText;
    }
    else
    {
        m_mapVertexDefines.insert({strText, true});
    }
}

void Shader::setGeometryDefineText(const std::string &strKey, const std::string &strText)
{
    if(m_mapGeometryDefines.find(strKey) != m_mapGeometryDefines.end())
    {
        m_mapGeometryDefines.at(strKey).strText = strText;
    }
    else
    {
        m_mapGeometryDefines.insert({strText, true});
    }
}

void Shader::addUniformLocation(const std::string &strUniformName)
{
    //  Inverse condition for optimized branch prediction
    if(m_mapUniformsLocations.find(strUniformName) != m_mapUniformsLocations.end())
    {
        return;
    }
    else
    {
        const GLint iLocation = glGetUniformLocation(m_uiProgram, strUniformName.c_str());
        m_mapUniformsLocations[strUniformName] = iLocation;
    }
}

void Shader::setIntUniform(const std::string &strUniformName, const int &iValue)
{
    addUniformLocation(strUniformName);
    glUniform1i(m_mapUniformsLocations[strUniformName], iValue);
}

void Shader::setFloatUniform(const std::string &strUniformName, const float &fValue)
{
    addUniformLocation(strUniformName);
    glUniform1f(m_mapUniformsLocations[strUniformName], fValue);
}

void Shader::setVec2Uniform(const std::string &strUniformName, const glm::vec2 &vValue)
{
    addUniformLocation(strUniformName);
    glUniform2f(m_mapUniformsLocations[strUniformName], vValue.x, vValue.y);
}

void Shader::addVec3Uniform(const std::string &strUniformName, const glm::vec3 &vValue)
{
    addUniformLocation(strUniformName);
    glUniform3f(m_mapUniformsLocations[strUniformName], vValue.x, vValue.y, vValue.z);
}

void Shader::setVec4Uniform(const std::string &strUniformName, const glm::vec4 &vValue)
{
    addUniformLocation(strUniformName);
    glUniform4f(m_mapUniformsLocations[strUniformName], vValue.x, vValue.y, vValue.z, vValue.w);
}

void Shader::addMat3Uniform(const std::string &strUniformName, const glm::mat3 &mValue)
{
    addUniformLocation(strUniformName);
    glUniformMatrix3fv(m_mapUniformsLocations[strUniformName], 1, GL_FALSE, glm::value_ptr(mValue));
}

void Shader::addMat4Uniform(const std::string &strUniformName, const glm::mat4 &mValue)
{
    addUniformLocation(strUniformName);
    glUniformMatrix4fv(m_mapUniformsLocations[strUniformName], 1, GL_FALSE, glm::value_ptr(mValue));
}

/*
 * Compile vertex, fragment and gemoetry shaders with source code given
 * Link the shader program
 * */
bool Shader::compileSourceCode(const std::string &strVertexShaderSource, const std::string &strFragmentShaderSource, const std::string &strGeometryShaderSource)
{
    GLint iSuccess;
    GLchar acInfoLog[512];

    const GLchar *pcVertexShaderSource = strVertexShaderSource.c_str();
    const GLchar *pcFragmentShaderSource = strFragmentShaderSource.c_str();
    const GLchar *pcGeometryShaderSource = strGeometryShaderSource.c_str();

    /*
     * Vertex shader compilation
     * */
    GLuint uiVertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(uiVertex, 1, &pcVertexShaderSource, NULL);
    glCompileShader(uiVertex);

    glGetShaderiv(uiVertex, GL_COMPILE_STATUS, &iSuccess);
    if(iSuccess == GL_FALSE)
    {
        glDeleteShader(uiVertex);

        glGetShaderInfoLog(uiVertex, 512, NULL, acInfoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << m_strVertexPath << ' ' << acInfoLog << std::endl;
        return false;
    }

    /*
     * Fragment shader compilation
     * */
    uiFragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(uiFragment, 1, &pcFragmentShaderSource, NULL);
    glCompileShader(uiFragment);

    glGetShaderiv(uiFragment, GL_COMPILE_STATUS, &iSuccess);
    if(iSuccess == GL_FALSE)
    {
        glDeleteShader(uiVertex);
        glDeleteShader(uiFragment);

        glGetShaderInfoLog(uiFragment, 512, NULL, acInfoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << m_strFragmentPath << ' ' << acInfoLog << std::endl;
        return false;
    }

    if(strGeometryShaderSource.size() != 0)
    {
        /*
         * Geometry shader compilation
         * */
        uiGeometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(uiGeometry, 1, &pcGeometryShaderSource, NULL);
        glCompileShader(uiGeometry);

        glGetShaderiv(uiGeometry, GL_COMPILE_STATUS, &iSuccess);
        if(iSuccess == GL_FALSE)
        {
            glDeleteShader(uiVertex);
            glDeleteShader(uiFragment);
            glDeleteShader(uiGeometry);

            glGetShaderInfoLog(uiGeometry, 512, NULL, acInfoLog);
            std::cerr << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << m_strGeometryPath << ' ' << acInfoLog << std::endl;
            return false;
        }
    }

    /*
     * Program linking
     * */
    //  Create program, delete it if already created
    if(m_bInitialised == true)
    {
        glDeleteProgram(m_uiProgram);
    }
    m_uiProgram = glCreateProgram();

    //  Attach shaders
    glAttachShader(m_uiProgram, uiVertex);
    glAttachShader(m_uiProgram, uiFragment);
    if(strGeometryShaderSource.size() != 0)
    {
        glAttachShader(m_uiProgram, uiGeometry);
    }

    //  Link
    glLinkProgram(m_uiProgram);

    //  Clean
    glDeleteShader(uiVertex);
    glDeleteShader(uiFragment);
    glDeleteShader(uiGeometry);

    glGetProgramiv(m_uiProgram, GL_LINK_STATUS, &iSuccess);
    if(iSuccess == GL_FALSE)
    {
        glDeleteProgram(m_uiProgram);

        glGetProgramInfoLog(m_uiProgram, 512, NULL, acInfoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << m_strVertexPath << ' ' << acInfoLog << std::endl;
        return false;
    }

    return true;
}

/*
 * Load vertex & fragment shader source code from files
 * files paths must have been stored inside the attributes
 * */
bool Shader::loadSourceFromFiles(std::string &strVertexShaderSource, std::string &strFragmentShaderSource, std::string &strGeometryShaderSource)
{
    std::ifstream vertexShaderFile;
    std::ifstream fragmentShaderFile;
    std::ifstream geometryShaderFile;

    vertexShaderFile.exceptions(std::ifstream::badbit);
    fragmentShaderFile.exceptions(std::ifstream::badbit);
    geometryShaderFile.exceptions(std::ifstream::badbit);
    try
    {
        vertexShaderFile.open(strVertexShaderSource.c_str());
        fragmentShaderFile.open(strFragmentShaderSource.c_str());
        geometryShaderFile.open(strGeometryShaderSource.c_str());

        std::stringstream vertexShaderStream;
        std::stringstream fragmentShaderStream;
        std::stringstream geometryShaderStream;

        // Add Shader Defines to the code
        if(m_mapFragmentDefines.empty() == false)
        {
            //  Assume that GLSL version is not written yet
            fragmentShaderStream << GLSL_VERSION << std::endl;
            for(std::unordered_map<std::string, bool>::const_iterator it = m_mapFragmentDefines.cbegin(); it != m_mapFragmentDefines.cend(); ++it)
            {
                //  If defined, write text
                if(it->second.bEnabled == true)
                {
                    fragmentShaderStream << "#define " << it->second.strText << std::endl;
                }
            }
        }
        if(m_mapVertexDefines.empty() == false)
        {
            //  Assume that GLSL version is not written yet
            vertexShaderStream << GLSL_VERSION << std::endl;
            for(std::unordered_map<std::string, bool>::const_iterator it = m_mapVertexDefines.cbegin(); it != m_mapVertexDefines.cend(); ++it)
            {
                //  If defined, write text
                if(it->second.bEnabled == true)
                {
                    vertexShaderStream << "#define " << it->second.strText << std::endl;
                }
            }
        }
        if(m_mapGeometryDefines.empty() == false)
        {
            //  Assume that GLSL version is not written yet
            fragmentShaderStream << GLSL_VERSION << std::endl;
            for(std::unordered_map<std::string, bool>::const_iterator it = m_mapGeometryDefines.cbegin(); it != m_mapGeometryDefines.cend(); ++it)
            {
                //  If defined, write text
                if(it->second.bEnabled == true)
                {
                    fragmentShaderStream << "#define " << it->second.strText << std::endl;
                }
            }
        }

        vertexShaderStream << vertexShaderFile.rdbuf();
        fragmentShaderStream << fragmentShaderFile.rdbuf();
        geometryShaderStream << geometryShaderFile.rdbuf();

        vertexShaderFile.close();
        fragmentShaderFile.close();
        geometryShaderFile.close();

        strVertexShaderSource = vertexShaderStream.str();
        strFragmentShaderSource = fragmentShaderStream.str();
        strGeometryShaderSource = geometryShaderStream.str();
    }
    catch(std::ifstream::failure e)
    {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << m_strVertexPath << ' ' << std::endl;
        return false;
    }

    return true;
}
