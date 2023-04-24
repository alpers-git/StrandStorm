#include <Renderer.hpp>
#include <Scene.hpp>

void Renderer::Initialize()
{
    glLineWidth(2.0f); $gl_chk
    glEnable(GL_DEPTH_TEST); $gl_chk
    
    // enable alpha blending
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); $gl_chk

    hairProg.CreatePipelineFromFiles(
        "shaders/hair.vert",
        "shaders/hair.frag");
    hairProg.SetClearColor({0.8f, 0.8f, 0.8f, 0.0f});

    surfaceProg.CreatePipelineFromFiles(
        "shaders/surface.vert",
        "shaders/surface.frag");
    surfaceProg.SetClearColor({0.8f, 0.8f, 0.8f, 0.0f});

    shadowProg.CreatePipelineFromFiles(
        "shaders/shadow.vert",
        "shaders/shadow.frag");
    shadowProg.SetClearColor({0.8f, 0.8f, 0.8f, 0.0f});

    opacityShadowProg.CreatePipelineFromFiles(
        "shaders/opacity_sh.vert",
        "shaders/opacity_sh.frag");
    opacityShadowProg.SetClearColor({0.0f, 0.0f, 0.0f, 0.0f});

    scene->init(*this);

    // Initialize hair generation / interpolation compute shader
    csHair.compile("shaders/hair_gen.comp");
    scene->hairMesh.bindToComputeShader(csHair);
}

void Renderer::RenderFirstPass()
{
    //render shadow map
    shadowProg.Use();
    shadowProg.SetUniform("to_clip_space", scene->light.CalculateLightSpaceMatrix());// mvp
    scene->light.shadowTexture->Render([&]() {
            scene->surface->mesh.draw(shadowProg);
            scene->hairMesh.draw(shadowProg);//to be removed when opacity shadowmaps are done
        });

    //render depthTexture for opacity shadowmap
    auto& depthTex =  scene->light.opacityShadowMaps.depthTex;
    shadowProg.Use();
    shadowProg.SetUniform("to_clip_space", scene->light.CalculateLightSpaceMatrix());
    depthTex->Render([&]() {
            scene->hairMesh.draw(shadowProg);
        });
    //render opacitymaps for opacity shadowmap
    scene->light.opacityShadowMaps.dirty = true;
    if(scene->light.opacityShadowMaps.dirty)
    {
        scene->light.opacityShadowMaps.dirty = false;
        opacityShadowProg.Use();
        opacityShadowProg.SetUniform("to_clip_space", scene->light.CalculateLightSpaceMatrix());
        opacityShadowProg.SetUniform("to_tex_space", scene->light.CalculateLightTexSpaceMatrix());
        opacityShadowProg.SetUniform("depth_map", (int)depthTex->texUnit - GL_TEXTURE0, false);
        opacityShadowProg.SetUniform("dk", scene->light.opacityShadowMaps.dk, false);
        scene->light.opacityShadowMaps.opacitiesTex->Render([&]() {
                opacityShadowProg.Clear();
                glDisable(GL_DEPTH_TEST) $gl_chk;
                glEnable(GL_BLEND) $gl_chk;
                glBlendFunc(GL_ONE, GL_ONE) $gl_chk;
                glBlendEquation(GL_FUNC_ADD) $gl_chk;
                depthTex->Bind();
                scene->hairMesh.draw(opacityShadowProg);
                glDisable(GL_BLEND) $gl_chk;
                glEnable(GL_DEPTH_TEST) $gl_chk;
            });
    }
} 

void Renderer::RenderMainPass()
{
    RenderSurfaces();
    RenderHairs();
}

void Renderer::Render()
{
    this->frameCount += 1;
    
    // Run hair generation compute shader
    csHair.bindBuffers();
    csHair.run({std::max(scene->hairMesh.numControlHairs(), scene->hairMesh.numTris()), 1, 1});

    shadowProg.Clear();
    RenderFirstPass();
    hairProg.Clear();
    RenderMainPass();
}

void Renderer::OnWindowResize(int width, int height)
{
    this->windowSize = {width, height};
    glViewport(0, 0, width, height); $gl_chk
}

void Renderer::OnMouseMove(double x, double y)
{
    mouseInteraction.delta = {x - mouseInteraction.lastPos.x, y - mouseInteraction.lastPos.y};
    mouseInteraction.lastPos = {x, y};

    if(mouseInteraction.leftButton)
        scene->cam.orient(mouseInteraction.delta);// Im not correct fix me
    if(mouseInteraction.rightButton)
        scene->cam.zoom(mouseInteraction.delta.y * 0.01f);
    if(mouseInteraction.middleButton)
        scene->cam.strafe(mouseInteraction.delta * 0.01f);
}

void Renderer::OnMouseButton(int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) 
        mouseInteraction.leftButton = true;
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        mouseInteraction.leftButton = false;
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        mouseInteraction.rightButton = true;
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        mouseInteraction.rightButton = false;
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
        mouseInteraction.middleButton = true;
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
        mouseInteraction.middleButton = false;
}

void Renderer::RenderHairs()
{

    hairProg.Use();

    hairProg.SetUniform("uTModel", glm::mat4(1.0f));
    hairProg.SetUniform("uTView", scene->cam.view());
    hairProg.SetUniform("uTProj", scene->cam.proj({windowSize}));
    hairProg.SetUniform("uTLight", scene->light.CalculateLightTexSpaceMatrix());
    hairProg.SetUniform("hair_color", scene->hairMesh.color, false);

    auto& depthTex =  scene->light.opacityShadowMaps.depthTex;
    depthTex->Bind();
    hairProg.SetUniform("depthMap", (int)depthTex->texUnit - GL_TEXTURE0);

    auto& opacitiesTex =  scene->light.opacityShadowMaps.opacitiesTex;
    opacitiesTex->Bind();
    hairProg.SetUniform("opacityMaps", (int)opacitiesTex->texUnit - GL_TEXTURE0, false);
    hairProg.SetUniform("dk", scene->light.opacityShadowMaps.dk);
    hairProg.SetUniform("shadows_enabled", scene->hairMesh.shadowsEnable);
    
    glm::mat4 viewInverse = glm::inverse(scene->cam.view());
    hairProg.SetUniform("viewInverse", viewInverse);
    hairProg.SetUniform("light_dir", scene->light.dir);
    hairProg.SetUniform("shininess", scene->hairMesh.shininess,false);
    hairProg.SetUniform("specular", scene->hairMesh.specular,false);
    hairProg.SetUniform("ambient", scene->hairMesh.ambient,false);

    scene->hairMesh.lut0->Bind();
    hairProg.SetUniform("lut0", (int)scene->hairMesh.lut0->texUnit - GL_TEXTURE0, false);
    scene->hairMesh.lut1->Bind();
    hairProg.SetUniform("lut1", (int)scene->hairMesh.lut1->texUnit - GL_TEXTURE0, false);

    hairProg.SetUniform("shadingModel", scene->hairMesh.shadingModel);

    // Marschner LUT parameters 
    hairProg.SetUniform("diffuseFalloff",scene->hairMesh.diffuseFalloff,false);
    hairProg.SetUniform("diffuseAzimuthFalloff",scene->hairMesh.diffuseAzimuthFalloff,false);
    hairProg.SetUniform("scaleDiffuse",scene->hairMesh.scaleDiffuse,false);
    hairProg.SetUniform("scaleR",scene->hairMesh.scaleR,false);
    hairProg.SetUniform("scaleTT",scene->hairMesh.scaleTT,false);
    hairProg.SetUniform("scaleTRT",scene->hairMesh.scaleTRT,false);

    // Marschner procedural parameters
    hairProg.SetUniform("roughness",glm::radians(scene->hairMesh.roughness),false);
    hairProg.SetUniform("shift",glm::radians(scene->hairMesh.shift),false);
    hairProg.SetUniform("refractiveIndex",scene->hairMesh.refractiveIndex,false);
    hairProg.SetUniform("procScaleR",scene->hairMesh.procScaleR,false);
    hairProg.SetUniform("procScaleTT",scene->hairMesh.procScaleTT,false);
    hairProg.SetUniform("procScaleTRT",scene->hairMesh.procScaleTRT,false);

    scene->hairMesh.draw(hairProg); //todo index this into an array and loop over it
}


void Renderer::RenderSurfaces()
{
    for(auto& surface : scene->sceneObjects)
        RenderSurface(*surface, surfaceProg);
    // RenderSurface(scene->surface.mesh, surfaceProg);
    // RenderSurface(scene->dummy.mesh, surfaceProg);
}

void Renderer::RenderSurface(SceneObject& sceneObj, OpenGLProgram& prog)
{
    prog.Use();
    glm::mat4 IDENTITY_MAT4  = glm::mat4(1.0f);
    glm::mat4 model_transform = glm::translate(IDENTITY_MAT4, sceneObj.position) 
                            * glm::eulerAngleZYX(glm::radians(sceneObj.rotation.z), 
                                                glm::radians(sceneObj.rotation.y), 
                                                glm::radians(sceneObj.rotation.x)) 
                            * glm::scale(IDENTITY_MAT4, sceneObj.scale);

    glm::mat4 to_view_space = scene->cam.view() * model_transform;
    glm::mat4 to_clip_space = scene->cam.proj({windowSize}) * to_view_space;
    glm::mat3 normals_to_view_space = glm::mat3(glm::transpose(glm::inverse(to_view_space)));
    prog.SetUniform("to_clip_space", to_clip_space);// mvp
    prog.SetUniform("to_view_space", to_view_space);//mv
    prog.SetUniform("normals_to_view_space", normals_to_view_space);//mv for normals
    prog.SetUniform("to_world_space",model_transform);//m For future use
    prog.SetUniform("to_light_view_space", scene->light.CalculateLightTexSpaceMatrix());

    surfaceProg.SetUniform("shadow_map", (int)scene->light.shadowTexture->texUnit - GL_TEXTURE0);
    scene->light.shadowTexture->Bind();

    prog.SetUniform("light_dir", scene->light.dir);
    prog.SetUniform("light_color", scene->light.color);
    prog.SetUniform("light_intensity", scene->light.intensity);
    prog.SetUniform("ambient", sceneObj.mesh.material.ambient);
    prog.SetUniform("diffuse", sceneObj.mesh.material.diffuse);
    prog.SetUniform("specular", sceneObj.mesh.material.specular);
    prog.SetUniform("shininess", sceneObj.mesh.material.shininess);

    sceneObj.mesh.draw(prog);
}

void Renderer::PostPhysicsSync()
{
    scene->hairMesh.updateBuffer();
}