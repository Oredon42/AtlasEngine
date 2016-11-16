#include "include/shader.h"

Shader::Shader() :
    m_nb_dirlights(0),
    m_nb_pointlights(0),
    m_nb_spotlights(0),
    m_initialised(GL_FALSE)
{

}

/*
 * Create shader according to its shader type
 * Shader code is divided into blocks that will be reunited
 * */
Shader::Shader(const ShaderType &shader_type, RenderingMethod rendering_method, const GLuint &nb_dirlights, const GLuint &nb_pointlights, const GLuint &nb_spotlights) :
    m_vertex_saved_path(""),
    m_fragment_saved_path(""),
    m_nb_dirlights(nb_dirlights),
    m_nb_pointlights(nb_pointlights),
    m_nb_spotlights(nb_spotlights),
    m_initialised(GL_FALSE)
{
    std::string vertex_code,
                fragment_code;

    generateShaderCode(shader_type, rendering_method, vertex_code, fragment_code);
    compileSourceCode(vertex_code.c_str(), fragment_code.c_str());

    m_initialised = GL_TRUE;
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

void Shader::init(const ShaderType &shader_type, RenderingMethod r, const GLuint &nb_dirlights, const GLuint &nb_pointlights, const GLuint &nb_spotlights)
{
    m_vertex_saved_path = "";
    m_fragment_saved_path = "";

    m_nb_dirlights = nb_dirlights;
    m_nb_pointlights = nb_pointlights;
    m_nb_spotlights = nb_spotlights;

    std::string vertex_code,
                fragment_code;

    generateShaderCode(shader_type, r, vertex_code, fragment_code);
    compileSourceCode(vertex_code.c_str(), fragment_code.c_str());

    m_initialised = GL_TRUE;
}

void Shader::init(const std::string &vertexPath, const std::string &fragmentPath, std::string path)
{
    m_vertex_saved_path = vertexPath;
    m_fragment_saved_path = fragmentPath;

    m_vertex_saved_path = path + m_vertex_saved_path;
    m_fragment_saved_path = path + m_fragment_saved_path;

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
    //std::cout << v_shader_code << "##################################################" << std::endl;
    //std::cout << f_shader_code << "--------------------------------------------------" << std::endl;

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
void Shader::generateShaderCode(const ShaderType &shader_type, RenderingMethod rendering_method, std::string &vertex_code, std::string &fragment_code)
{
    std::string version_block = "#version 330 core\n";

    std::string v_layout_block = "layout (location = 0) in vec3 position;\nlayout (location = 1) in vec3 normal;\nlayout (location = 2) in vec2 texCoords;\n";
    std::string v_out_block = "out vec3 FragPos;\nout vec3 Normal;\n";
    std::string v_uniform_block = "uniform mat4 model;\nuniform mat4 view;\nuniform mat4 projection;\n";
    std::string v_main_block = "void main()\n{\n";

    std::string f_layout_block = "layout (location = 0) out vec3 gPosition;\nlayout (location = 1) out vec3 gNormal;\nlayout (location = 2) out vec4 gAlbedoSpec;\n";
    std::string f_define_block;
    std::string f_in_block = "in vec3 FragPos;\nin vec3 Normal;\n";
    std::string f_out_block;
    std::string f_uniform_block;
    std::string f_material_block = "struct Material\n{\n";
    std::string f_main_block = "void main()\n{\n";

    std::string f_dirlight_struct_block;
    std::string f_pointlight_struct_block;
    std::string f_spotlight_struct_block;
    std::string f_dirlight_func_block;
    std::string f_pointlight_func_block;
    std::string f_spotlight_func_block;

    if(shader_type.animated)
        v_main_block += "mat4 bone_transform = gBones[BoneIDs[0]] * Weights[0];\nbone_transform += gBones[BoneIDs[1]] * Weights[1];\nbone_transform += gBones[BoneIDs[2]] * Weights[2];\nbone_transform += gBones[BoneIDs[3]] * Weights[3];\nvec4 pos_L = bone_transform * vec4(position, 1.0);\ngl_Position = projection * view * model * pos_L;\nFragPos = vec4(pos_L).xyz;\nvec4 normal_L = bone_transform * vec4(normal, 0.0);\nNormal = normal_L.xyz;\n";
    else
        v_main_block += "vec4 worldPos = model * vec4(position, 1.0f);\nFragPos = worldPos.xyz; \ngl_Position = projection * view * worldPos;\nmat3 normalMatrix = transpose(inverse(mat3(model)));\nNormal = normalMatrix * normal;\n";

    if(shader_type.texture || shader_type.specular || shader_type.normal)
    {
        v_out_block += "out vec2 TexCoords;\n";
        v_main_block += "TexCoords = texCoords;\n";
        f_in_block += "in vec2 TexCoords;\n";
    }

    if(shader_type.texture)
        f_material_block += "sampler2D texture_diffuse1;\n";
    else
        f_material_block += "vec3 diffuse;\n";

    if(shader_type.specular)
        f_material_block += "sampler2D texture_specular1;\n";
    else
        f_material_block += "vec3 specular;\n";

    if(shader_type.normal)
    {
        v_layout_block += "layout (location = 3) in vec3 tangent;\nlayout (location = 4) in vec3 bitangent;\n";
        f_material_block += "sampler2D texture_normal1;\n";
        f_main_block += "vec3 norm = texture(material.texture_normal1, TexCoords).rgb;\n";

        if(shader_type.animated)
        {
            v_layout_block += "layout (location = 5) in ivec4 BoneIDs;\nlayout (location = 6) in vec4 Weights;\n";
            v_uniform_block += "const int MAX_BONES = 100;\nuniform mat4 gBones[MAX_BONES];\n";
        }
    }
    else
    {
        f_main_block += "vec3 norm = normalize(Normal);\n";

        if(shader_type.animated)
        {
            v_layout_block += "layout (location = 3) in ivec4 BoneIDs;\nlayout (location = 4) in vec4 Weights;\n";
            v_uniform_block += "const int MAX_BONES = 100;\nuniform mat4 gBones[MAX_BONES];\n";
        }
    }

    if(rendering_method == FORWARD)
    {
        std::stringstream   dirlights_define_ss,
                            pointlights_define_ss,
                            spotlights_define_ss;

        // Generate lights func
        std::string l_material_diffuse,
                    l_material_specular;

        if(shader_type.texture)
            l_material_diffuse = "vec3(texture(material.texture_diffuse1, TexCoords));\n";
        else
            l_material_diffuse = "material.diffuse;\n";

        if(shader_type.specular)
            l_material_specular = "vec3(texture(material.texture_specular1, TexCoords));\n";
        else
            l_material_specular = "material.specular;\n";

        std::string l_dirlight_declaration = "vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)\n{\n",
                    l_pointlight_declaration = "vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)\n{\n",
                    l_spotlight_declaration = "vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)\n{\n",
                    l_dir = "vec3 lightDir = normalize(-light.direction);\n",
                    l_diff = "float diff = max(dot(normal, lightDir), 0.0);\n",
                    l_refl = "vec3 reflectDir = reflect(-lightDir, normal);\n",
                    l_spec = "float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);\n",
                    l_distance = "float distance = length(light.position - fragPos);\n",
                    l_attenuation = "float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));\n",
                    l_theta = "float theta = dot(lightDir, normalize(-light.direction));\n",
                    l_epsilon = "float epsilon = light.cutOff - light.outerCutOff;\n",
                    l_intensity = "float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);\n",
                    l_ambient = "vec3 ambient = light.ambient * " + l_material_diffuse,
                    l_diffuse = "vec3 diffuse = light.diffuse * diff * " + l_material_diffuse,
                    l_specular = "vec3 specular = light.specular * spec * " + l_material_specular,
                    l_calc_point_light = "ambient *= attenuation;\ndiffuse *= attenuation;\nspecular *= attenuation;\n",
                    l_calc_spot_light,// = "ambient *= attenuation * intensity;\ndiffuse *= attenuation * intensity;\nspecular *= attenuation * intensity;\n",
                    l_return = "return (ambient + diffuse + specular);\n}\n";

        f_main_block += "vec3 viewDir = normalize(viewPos - FragPos);\nvec3 result = vec3(0, 0, 0);\n";

        f_uniform_block += "uniform vec3 viewPos;\n";
        if(m_nb_dirlights > 0)
        {
            dirlights_define_ss << "#define NB_DIRLIGHTS " << m_nb_dirlights << "\n";
            f_define_block = dirlights_define_ss.str();
            f_dirlight_struct_block += "struct DirLight {\nvec3 direction;\nvec3 ambient;\nvec3 diffuse;\nvec3 specular;\n};\n";
            f_dirlight_func_block = l_dirlight_declaration + l_dir + l_diff + l_refl + l_spec + l_ambient + l_diffuse + l_specular + l_return;
            f_main_block += "for(int i = 0; i < NB_DIRLIGHTS; i++)\nresult += CalcDirLight(dirLights[i], norm, viewDir);\n";
            f_uniform_block += "uniform DirLight dirLights[NB_DIRLIGHTS];\n";
        }
        l_dir = "vec3 lightDir = normalize(light.position - fragPos);\n";
        if(m_nb_pointlights > 0)
        {
            pointlights_define_ss << "#define NB_POINTLIGHTS " << m_nb_pointlights << "\n";
            f_define_block += pointlights_define_ss.str();
            f_pointlight_struct_block = "struct PointLight {\nvec3 position;\nfloat constant;\nfloat linear;\nfloat quadratic;\nvec3 ambient;\nvec3 diffuse;\nvec3 specular;\n};\n";
            f_pointlight_func_block = l_pointlight_declaration + l_dir + l_diff + l_refl + l_spec + l_distance + l_attenuation + l_ambient + l_diffuse + l_specular + l_calc_point_light + l_return;
            f_main_block += "for(int i = 0; i < NB_POINTLIGHTS; i++)\nresult += CalcPointLight(pointLights[i], norm, FragPos, viewDir);\n";
            f_uniform_block += "uniform PointLight pointLights[NB_POINTLIGHTS];\n";
        }
        if(m_nb_spotlights > 0)
        {
            spotlights_define_ss << "#define NB_SPOTLIGHTS " << m_nb_spotlights << "\n";
            f_define_block += spotlights_define_ss.str();
            f_spotlight_struct_block += "struct SpotLight {\nvec3 position;\nvec3 direction;\nfloat cutOff;\nfloat outerCutOff;\nfloat constant;\nfloat linear;\nfloat quadratic;\nvec3 ambient;\nvec3 diffuse;\nvec3 specular;\n};\n";
            f_spotlight_func_block = l_spotlight_declaration + l_dir + l_diff + l_refl + l_spec + l_distance + l_attenuation + l_theta + l_epsilon + l_intensity + l_ambient + l_diffuse + l_specular + l_calc_spot_light + l_return;
            f_main_block += "for(int i = 0; i < NB_SPOTLIGHTS; i++)\nresult += CalcSpotLight(spotLights[i], norm, FragPos, viewDir);\n";
            f_uniform_block += "uniform SpotLight spotLights[NB_SPOTLIGHTS];\n";
        }
        f_main_block += "color = vec4(result, 1.0);\n";

        f_out_block += "out vec4 color;\n";
    }
    else
    {
        f_main_block += "gPosition = FragPos;\n";
        /*
         * DEFFERED
         * */
        if(shader_type.texture)
            f_main_block += "gAlbedoSpec.rgb = texture(material.texture_diffuse1, TexCoords).rgb;\n";
        else
            f_main_block += "gAlbedoSpec.rgb = material.diffuse;\n";

        if(shader_type.specular)
            f_main_block += "gAlbedoSpec.a = texture(material.texture_specular1, TexCoords).r;\n";
        else
            f_main_block += "gAlbedoSpec.a = material.specular.x;\n";

        if(shader_type.normal)
           f_main_block += "gNormal = normalize(norm * 2.0 - 1.0);";
        else
            f_main_block += "gNormal = norm;\n";
    }

    f_material_block += "float shininess;\n};\nuniform Material material;\n";
    f_main_block += "}\n";
    v_main_block += "}\n";

    std::string str_vertex_code = version_block + v_layout_block + v_out_block + v_uniform_block + v_main_block,
                str_fragment_code;

    if(rendering_method == DEFFERED)
        str_fragment_code = version_block + f_layout_block + f_in_block + f_material_block + f_main_block;
    else
        str_fragment_code = version_block + f_define_block + f_in_block + f_out_block + f_material_block + f_dirlight_struct_block + f_pointlight_struct_block + f_spotlight_struct_block + f_dirlight_func_block + f_pointlight_func_block + f_spotlight_func_block + f_uniform_block + f_main_block;

    vertex_code = str_vertex_code.c_str();
    fragment_code = str_fragment_code.c_str();
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
