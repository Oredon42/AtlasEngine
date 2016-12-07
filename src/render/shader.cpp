#include "include/render/shader.h"

Shader::Shader() :
    m_nb_dirlights(0),
    m_nb_pointlights(0),
    m_nb_spotlights(0),
    m_initialised(GL_FALSE)
{

}

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath, std::string path) :
    m_vertex_saved_path(vertexPath),
    m_fragment_saved_path(fragmentPath),
    m_initialised(GL_FALSE)
{
    m_vertex_saved_path = path + m_vertex_saved_path;
    m_fragment_saved_path = path + m_fragment_saved_path;

    std::string vertex_code,
                fragment_code;

    loadSourceFromFiles(vertex_code, fragment_code);
    compileSourceCode(vertex_code.c_str(), fragment_code.c_str());

    m_initialised = GL_TRUE;
}

void Shader::initForward(const ShaderType &shader_type, const GLuint &nb_dirlights, const GLuint &nb_pointlights, const GLuint &nb_spotlights)
{
    m_vertex_saved_path = "";
    m_fragment_saved_path = "";

    m_nb_dirlights = nb_dirlights;
    m_nb_pointlights = nb_pointlights;
    m_nb_spotlights = nb_spotlights;

    std::string vertex_code,
                fragment_code;

    generateForwardCode(shader_type, vertex_code, fragment_code);
    compileSourceCode(vertex_code.c_str(), fragment_code.c_str());

    m_initialised = GL_TRUE;
}

void Shader::initGeometry(const ShaderType &shader_type)
{
    m_vertex_saved_path = "";
    m_fragment_saved_path = "";

    m_nb_dirlights = 0;
    m_nb_pointlights = 0;
    m_nb_spotlights = 0;

    std::string vertex_code,
                fragment_code;

    generateGeometryCode(shader_type, vertex_code, fragment_code);
    compileSourceCode(vertex_code.c_str(), fragment_code.c_str());

    m_initialised = GL_TRUE;
}

void Shader::initLighting(const GLuint &nb_dirlights, const GLuint &nb_pointlights, const GLuint &nb_spotlights)
{
    m_vertex_saved_path = "";
    m_fragment_saved_path = "";

    m_nb_dirlights = nb_dirlights;
    m_nb_pointlights = nb_pointlights;
    m_nb_spotlights = nb_spotlights;

    std::string vertex_code,
                fragment_code;

    generateLightingCode(vertex_code, fragment_code);
    compileSourceCode(vertex_code.c_str(), fragment_code.c_str());

    m_initialised = GL_TRUE;
}

void Shader::init(const std::string &vertexPath, const std::string &fragmentPath, std::string path)
{
    m_vertex_saved_path = vertexPath;
    m_fragment_saved_path = fragmentPath;

    //m_vertex_saved_path = path + m_vertex_saved_path;
    //m_fragment_saved_path = path + m_fragment_saved_path;

    std::string vertex_code,
                fragment_code;

    loadSourceFromFiles(vertex_code, fragment_code);
    compileSourceCode(vertex_code.c_str(), fragment_code.c_str());

    m_initialised = GL_TRUE;
}

void Shader::reload()
{
    std::string vertex_code,
                fragment_code;

    loadSourceFromFiles(vertex_code, fragment_code);
    compileSourceCode(vertex_code.c_str(), fragment_code.c_str());
}

void Shader::use()
{
    if(m_initialised)
        glUseProgram(m_program);
    else
        std::cerr << "ERROR::SHADER::USE The shader has to be initialised before it can be used" << std::endl;
}

/*
 * Compile vertex and fragment shaders with source code given
 * Link the shader program
 * */
GLboolean Shader::compileSourceCode(const GLchar *v_shader_code, const GLchar *f_shader_code)
{
    GLuint vertex, fragment;
    GLint success;
    GLchar info_log[512];

    /*
     * Vertex shader compilation
     * */
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &v_shader_code, NULL);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, info_log);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << m_vertex_saved_path << " " << info_log << std::endl;
        return GL_FALSE;
    }

    /*
     * Fragment shader compilation
     * */
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &f_shader_code, NULL);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, info_log);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << m_fragment_saved_path << " " << info_log << std::endl;
        return GL_FALSE;
    }

    /*
     * Program linking
     * */
    if(!m_initialised)
        m_program = glCreateProgram();
    glAttachShader(m_program, vertex);
    glAttachShader(m_program, fragment);
    glLinkProgram(m_program);

    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(m_program, 512, NULL, info_log);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << m_vertex_saved_path << " " << info_log << std::endl;
        return GL_FALSE;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return GL_TRUE;
}

/*
 * Load vertex & fragment shader source code from files
 * files paths must have been stored inside the Shader attributes
 * m_vertex_saved_path & m_fragment_saved_path
 * */
GLboolean Shader::loadSourceFromFiles(std::string &vertex_code, std::string &fragment_code)
{
    std::ifstream v_shader_file;
    std::ifstream f_shader_file;

    v_shader_file.exceptions(std::ifstream::badbit);
    f_shader_file.exceptions(std::ifstream::badbit);
    try
    {
        v_shader_file.open(m_vertex_saved_path.c_str());
        f_shader_file.open(m_fragment_saved_path.c_str());

        std::stringstream v_shader_stream, f_shader_stream;

        v_shader_stream << v_shader_file.rdbuf();
        f_shader_stream << f_shader_file.rdbuf();

        v_shader_file.close();
        f_shader_file.close();

        vertex_code = v_shader_stream.str();
        fragment_code = f_shader_stream.str();
    }
    catch(std::ifstream::failure e)
    {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << m_vertex_saved_path << " " << std::endl;
        return GL_FALSE;
    }

    return GL_TRUE;
}

/*
 * Generate the code of vertex and fragment shader
 * according to rendering_method
 * using the value of shader_type to generate material
 * */
void Shader::generateForwardCode(const ShaderType &shader_type, std::string &vertex_code, std::string &fragment_code)
{
    std::ifstream v_shader_file;
    std::ifstream f_shader_file;

    v_shader_file.exceptions(std::ifstream::badbit);
    f_shader_file.exceptions(std::ifstream::badbit);
    try
    {
        std::stringstream v_shader_stream, f_shader_stream;

        v_shader_stream << "#version 330 core\n\n";
        f_shader_stream << "#version 330 core\n\n";

        if(m_nb_pointlights > 0)
            f_shader_stream << "#define POINTLIGHT " << m_nb_pointlights << "\n";

        if(shader_type.texture)
        {
            v_shader_stream << "#define TEXTURE\n";
            f_shader_stream << "#define TEXTURE\n";
        }

        if(shader_type.normal)
        {
            v_shader_stream << "#define NORMAL\n";
            f_shader_stream << "#define NORMAL\n";
        }

        if(shader_type.specular)
        {
            v_shader_stream << "#define SPECULAR\n";
            f_shader_stream << "#define SPECULAR\n";
        }

        v_shader_file.open("shaders/metaforward.vert");
        f_shader_file.open("shaders/metaforward.frag");

        v_shader_stream << v_shader_file.rdbuf();
        f_shader_stream << f_shader_file.rdbuf();

        v_shader_file.close();
        f_shader_file.close();

        vertex_code = v_shader_stream.str();
        fragment_code = f_shader_stream.str();
    }
    catch(std::ifstream::failure e)
    {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << m_vertex_saved_path << " " << std::endl;
    }
}

void Shader::generateGeometryCode(const ShaderType &shader_type, std::string &vertex_code, std::string &fragment_code)
{
    std::ifstream v_shader_file;
    std::ifstream f_shader_file;

    v_shader_file.exceptions(std::ifstream::badbit);
    f_shader_file.exceptions(std::ifstream::badbit);
    try
    {
        std::stringstream v_shader_stream, f_shader_stream;

        v_shader_stream << "#version 330 core\n\n";
        f_shader_stream << "#version 330 core\n\n";

        if(shader_type.texture)
        {
            v_shader_stream << "#define TEXTURE\n";
            f_shader_stream << "#define TEXTURE\n";
        }

        if(shader_type.normal)
        {
            v_shader_stream << "#define NORMAL\n";
            f_shader_stream << "#define NORMAL\n";
        }

        if(shader_type.specular)
        {
            v_shader_stream << "#define SPECULAR\n";
            f_shader_stream << "#define SPECULAR\n";
        }

        v_shader_file.open("shaders/metagbuffer.vert");
        f_shader_file.open("shaders/metagbuffer.frag");

        v_shader_stream << v_shader_file.rdbuf();
        f_shader_stream << f_shader_file.rdbuf();

        v_shader_file.close();
        f_shader_file.close();

        vertex_code = v_shader_stream.str();
        fragment_code = f_shader_stream.str();
    }
    catch(std::ifstream::failure e)
    {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << m_vertex_saved_path << " " << std::endl;
    }
}

void Shader::generateLightingCode(std::string &vertex_code, std::string &fragment_code)
{
    std::ifstream v_shader_file;
    std::ifstream f_shader_file;

    v_shader_file.exceptions(std::ifstream::badbit);
    f_shader_file.exceptions(std::ifstream::badbit);
    try
    {
        std::stringstream v_shader_stream, f_shader_stream;

        v_shader_stream << "#version 330 core\n\n";
        f_shader_stream << "#version 330 core\n\n";

        if(m_nb_pointlights > 0)
            f_shader_stream << "#define POINTLIGHT " << m_nb_pointlights << "\n";

        v_shader_file.open("shaders/metalighting.vert");
        f_shader_file.open("shaders/metalighting.frag");

        v_shader_stream << v_shader_file.rdbuf();
        f_shader_stream << f_shader_file.rdbuf();

        v_shader_file.close();
        f_shader_file.close();

        vertex_code = v_shader_stream.str();
        fragment_code = f_shader_stream.str();
    }
    catch(std::ifstream::failure e)
    {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << m_vertex_saved_path << " " << std::endl;
    }
}


/*
 * Return the shader type of a Model
 * parameters are:
 *  -is animated   : if the mesh has bones
 *  -diffuse_size  : the size of the diffuse textures vector
 *  -specular_size : the size of the specular textures vector
 *  -normal_size   : the size of the normal maps vector
 * */
ShaderType getShaderType(const GLboolean &is_animated, const GLuint &diffuse_size, const GLuint &specular_size, const GLuint &normal_size)
{
    ShaderType shader_type;

    shader_type.texture = diffuse_size > 0;
    shader_type.specular = specular_size > 0;
    shader_type.normal = normal_size > 0;
    shader_type.animated = is_animated;

    shader_type.index = (shader_type.texture?1:0) + (shader_type.specular?2:0) + (shader_type.normal?4:0) + (shader_type.animated?8:0);

    return shader_type;
}
