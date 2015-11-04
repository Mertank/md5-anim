#include "ModelViewer.h"
#include "TextureManager.h"

/*
=============
ModelViewer::ModelViewer

	ModelViewer Constructor.
=============
*/
ModelViewer::ModelViewer( void ) :
	mainCamera( NULL ),
	currentModel( NULL ),
    currentModelText( NULL ),
    consolasFont( NULL ),
	projectionMatrix( 1.0 ),
	CPUSkinningProgram( 0 ),
    GPUSkinningProgram( 0 ),
    textProgram( 0 ),
	animateModel( true ), 
    modelIndex( 0 ),
	screenHeight( 0.0f ),
	screenWidth( 0.0f )
{}
/*
=============
ModelViewer::~ModelViewer

	ModelViewer Destructor.
=============
*/
ModelViewer::~ModelViewer( void ) {}
/*
=============
ModelViewer::initialization

	ModelViewer Initialization.
=============
*/
void ModelViewer::initialize( int w, int h ) {
	glClearColor( 0.6f, 0.6f, 0.6f, 1.0f );

	mainCamera = new glsh::FreeLookCamera( this );
	mainCamera->setPosition( glm::vec3( 0.0f, 0.0f, -500.0f ) );

	CPUSkinningProgram  = Program::CreateProgram( "assets/shaders/CPULightingVertex.glsl", "assets/shaders/LightingFragment.glsl" );
    GPUSkinningProgram  = Program::CreateProgram( "assets/shaders/GPULightingVertex.glsl", "assets/shaders/LightingFragment.glsl" );
    textProgram         = Program::CreateProgram( "assets/shaders/TextVertex.glsl", "assets/shaders/TextFragment.glsl" );

	glEnable( GL_DEPTH_TEST );
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
    currentModelText = new glsh::TextBatch();
    consolasFont = glsh::CreateFont( "assets/fonts/consolas15" );

	/****SET SHADER UNIFORMS*****/
	CPUSkinningProgram->SetUniform( "uModelMatrix", &glm::mat4(1.0)[0][0], 16 );
	GPUSkinningProgram->SetUniform( "uModelMatrix", &glm::mat4(1.0)[0][0], 16 );
	CPUSkinningProgram->SetUniform( "uNormalMatrix", &glm::mat3(1.0)[0][0], 12 );
	GPUSkinningProgram->SetUniform( "uNormalMatrix", &glm::mat3(1.0)[0][0], 12 );
	CPUSkinningProgram->SetUniform( "uMatColor", &glm::vec3(1.0, 0.0, 0.0)[0], 3 );
	GPUSkinningProgram->SetUniform( "uMatColor", &glm::vec3(1.0, 0.0, 0.0)[0], 3 );

	CPUSkinningProgram->SetUniform( "uLightColor", &glm::vec3( 1.0f, 1.0f, 1.0f )[0], 3 );
	GPUSkinningProgram->SetUniform( "uLightColor", &glm::vec3( 1.0f, 1.0f, 1.0f )[0], 3 );

	glm::vec3 lightDir = glm::vec3( -20.0f, 0.0f, 0.0f );
	lightDir = glm::normalize( lightDir );
	CPUSkinningProgram->SetUniform( "uLightDir", &lightDir[0], 3 );
	GPUSkinningProgram->SetUniform( "uLightDir", &lightDir[0], 3 );

    textProgram->SetUniform( "u_Tint", &glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f )[0], 4 );
    textProgram->SetUniform( "u_TexSampler", 0 );

	LoadModels();
}
/*
=============
ModelViewer::shutdown

	ModelViewer shutdown.
=============
*/
void ModelViewer::shutdown( void ) {
	delete CPUSkinningProgram;	
    delete GPUSkinningProgram;	
	delete textProgram;
	delete mainCamera;
	delete consolasFont;
	delete currentModelText;

	for ( std::vector<MD5Model*>::iterator model = models.begin();
		  model != models.end(); ++model ) {
		delete *model;
	}
	models.clear();
}
/*
=============
ModelViewer::resize

	ModelViewer resize.
=============
*/
void ModelViewer::resize( int w, int h ) {    
	screenHeight = ( float )h;
	screenWidth  = ( float )w;

	glViewport( 0, 0, w, h );

	projectionMatrix = glm::perspective( 45.0f, ( float )w / ( float )h, 1.0f, 1000.0f );	
	orthoMatrix      = glm::ortho( 0.0f, ( float )w, 0.0f, ( float )h, -1.0f, 1.0f );

    mainCamera->setPosition( glm::vec3( 0.0f, 50.0f, -200.0f ) );
	mainCamera->lookAt( 0.0f, 50.0f, 0.0f );
	mainCamera->setSpeed( 20.0f );

	CPUSkinningProgram->SetUniform( "uProjectionMatrix", &projectionMatrix[0][0], 16 );
	GPUSkinningProgram->SetUniform( "uProjectionMatrix", &projectionMatrix[0][0], 16 );
	textProgram->SetUniform( "u_ProjectionMatrix", &orthoMatrix[0][0], 16 );    

	UpdateCurrentModelInfo();
}
/*
=============
ModelViewer::draw

	ModelViewer render.
=============
*/
void ModelViewer::draw( void ) {
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	if ( currentModel ) {
        if ( currentModel->GetSkinningType() == CPU_SKINNING ) {
            CPUSkinningProgram->SetUniform( "uViewMatrix", &mainCamera->getViewMatrix()[0][0], 16 );
            CPUSkinningProgram->SetUniform( "uNormalMatrix", &glm::mat3(currentModel->GetModelMatrix() * mainCamera->getViewMatrix())[0][0], 12 );

		    currentModel->Render( CPUSkinningProgram );
        } else if ( currentModel->GetSkinningType() == GPU_SKINNING ) {
            GPUSkinningProgram->SetUniform( "uViewMatrix", &mainCamera->getViewMatrix()[0][0], 16 );
            GPUSkinningProgram->SetUniform( "uNormalMatrix", &glm::mat3(currentModel->GetModelMatrix() * mainCamera->getViewMatrix())[0][0], 12 );

		    currentModel->Render( GPUSkinningProgram );
        }
	}

    textProgram->Use();
    textProgram->SetUniform( "u_ModelMatrix", &currentModelText->GetModelMatrix()[0][0], 16 );
    currentModelText->DrawGeometry();
}
/*
=============
ModelViewer::update

	ModelViewer update.
=============
*/
bool ModelViewer::update( float dt ) {
	
	mainCamera->update( dt );	
	
	const glsh::Keyboard* kb = getKeyboard();
	
	if ( kb->keyPressed( glsh::KC_ESCAPE ) ) {
		return false; // request to exit
	} else if ( kb->keyPressed( glsh::KC_P ) ) { //Increase anim1 
		if ( currentModel ) {
			std::vector<int> animIndicies = currentModel->GetPlayingAnimationIndicies();
			++animIndicies[0];
			animIndicies[0] = ( animIndicies[0] >= ( int )currentModel->GetAnimationCount() ) ? -1 : animIndicies[0];
			if ( animIndicies[0] > -1 && animIndicies[1] > -1 ) {
				currentModel->PlayBlendedAnimation( animIndicies[0], animIndicies[1], 0.5f );
			} else if ( animIndicies[1] > -1 ) {
				currentModel->PlaySingleAnimation( animIndicies[1] );
			} else {
				currentModel->PlaySingleAnimation( animIndicies[0] );
			}

			UpdateCurrentModelInfo();
		}
	} else if ( kb->keyPressed( glsh::KC_O ) ) { //Decrease anim1
		if ( currentModel ) {
			std::vector<int> animIndicies = currentModel->GetPlayingAnimationIndicies();
			--animIndicies[0];
			animIndicies[0] = ( animIndicies[0] < -1 ) ? currentModel->GetAnimationCount() - 1 : animIndicies[0];
			if ( animIndicies[0] > -1 && animIndicies[1] > -1 ) {
				currentModel->PlayBlendedAnimation( animIndicies[0], animIndicies[1], 0.5f );
			} else if ( animIndicies[1] > -1 ) {
				currentModel->PlaySingleAnimation( animIndicies[1] );
			} else {
				currentModel->PlaySingleAnimation( animIndicies[0] );
			}
			UpdateCurrentModelInfo();
		}
	} else if ( kb->keyPressed( glsh::KC_I ) ) { //Increase anim2
		if ( currentModel ) {
			std::vector<int> animIndicies = currentModel->GetPlayingAnimationIndicies();
			++animIndicies[1];
			animIndicies[1] = ( animIndicies[1] >= ( int )currentModel->GetAnimationCount() ) ? -1 : animIndicies[1];
			if ( animIndicies[0] > -1 && animIndicies[1] > -1 ) {
				currentModel->PlayBlendedAnimation( animIndicies[0], animIndicies[1], 0.5f );
			} else if ( animIndicies[0] > -1 ) {
				currentModel->PlaySingleAnimation( animIndicies[0] );
			} else {
				currentModel->PlaySingleAnimation( animIndicies[1] );
			}
			UpdateCurrentModelInfo();
		}
	} else if ( kb->keyPressed( glsh::KC_U ) ) { //Decrease anim2
		if ( currentModel ) {
			std::vector<int> animIndicies = currentModel->GetPlayingAnimationIndicies();
			--animIndicies[1];
			animIndicies[1] = ( animIndicies[1] < -1 ) ? currentModel->GetAnimationCount() - 1 : animIndicies[1];
			if ( animIndicies[0] > -1 && animIndicies[1] > -1 ) {
				currentModel->PlayBlendedAnimation( animIndicies[0], animIndicies[1], 0.5f );
			} else if ( animIndicies[0] > -1 ) {
				currentModel->PlaySingleAnimation( animIndicies[0] );
			} else {
				currentModel->PlaySingleAnimation( animIndicies[1] );
			}
			UpdateCurrentModelInfo();
		}
	} else if ( kb->keyPressed( glsh::KC_Y ) ) { //Increase blend
		if ( currentModel ) {
			currentModel->SetBlendFactor( currentModel->GetBlendFactor() + 0.05f );
			UpdateCurrentModelInfo();
		}
	} else if ( kb->keyPressed( glsh::KC_T ) ) { //Decrease blend
		if ( currentModel ) {
			currentModel->SetBlendFactor( currentModel->GetBlendFactor() - 0.05f );
			UpdateCurrentModelInfo();
		}
	} else if ( kb->keyPressed( glsh::KC_K ) ) { //Increase model index
		++modelIndex;
		modelIndex = ( ( unsigned )modelIndex >= models.size() ) ? 0 : modelIndex;
		currentModel = models[modelIndex];
        currentModel->PlaySingleAnimation( 0 );
		UpdateCurrentModelInfo();
	} else if ( kb->keyPressed( glsh::KC_L ) ) { //Decrease model index
		--modelIndex;
		modelIndex = ( modelIndex < 0 ) ? models.size() - 1 : modelIndex;
		currentModel = models[modelIndex];
        currentModel->PlaySingleAnimation( 0 );
		UpdateCurrentModelInfo();
	} else if ( kb->keyPressed( glsh::KC_SPACE ) ) { //Toggle animation
		animateModel = !animateModel;
	} else if ( kb->keyPressed( glsh::KC_G ) ) { //Change skinning type
        currentModel->SetSkinningType( ( currentModel->GetSkinningType() == CPU_SKINNING ) ? GPU_SKINNING : CPU_SKINNING );
        UpdateCurrentModelInfo();
	}

	if ( currentModel && animateModel ) {
		currentModel->Update( dt );
	}

	return true; // request to keep going
}
/*
=============
ModelViewer::LoadModels

	Load the models stated in the assets folder.
=============
*/
void ModelViewer::LoadModels( void ) {
	//File with meshes to load
	std::string file = glsh::ReadTextFile( "assets/meshes.txt" );
	std::vector<std::string> lines = glsh::Split( file, '\n' );

	MD5Model* loadedModel = NULL;
	for ( std::vector<std::string>::iterator currentLine = lines.begin(); 
		  currentLine != lines.end(); ++currentLine ) {
		if ( currentLine->length() > 0 && ( *currentLine )[0] != '#' ) {
			std::string type = currentLine->substr( currentLine->length() - 4, currentLine->length() );

			if ( type.compare( "mesh" ) == 0 ) {
				loadedModel = MD5Model::CreateMD5ModelWithMesh( currentLine->c_str() );
				if ( loadedModel != NULL ) {
					models.push_back( loadedModel );
					loadedModel->SetRotation( -90, glm::vec3( 1.0f, 0.0f, 0.0f ) );
					loadedModel->RotateAround( 90, glm::vec3( 0.0f, 0.0f, 1.0f ) );
					loadedModel->SetMaterial( glm::vec3( 1.0f, 1.0f, 1.0f ) );
				}
			} else if ( type.compare( "anim" ) == 0 ) {
				loadedModel->AddAnimation( currentLine->c_str() );
			}
		}
	}

	if ( models.size() > 0 ) {
		currentModel = models[0];
	}
}
/*
=============
ModelViewer::UpdateCurrentModelInfo

	Updates the Model Info text.
=============
*/
void ModelViewer::UpdateCurrentModelInfo( void ) {
	if ( currentModel ) {

		std::vector<std::string> animNames = currentModel->GetPlayingAnimationNames();

		std::string modelInfo = "Mesh name: " + currentModel->GetModelName() + "\n";
		modelInfo += "Current First Animation: " + animNames[0] + "\n";
		modelInfo += "Current Second Animation: " + animNames[1] + "\n";
		modelInfo += "Current Blend Amount: " + std::to_string( currentModel->GetBlendFactor() ) + "\n"; 

        if ( currentModel->GetSkinningType() == CPU_SKINNING ) {
            modelInfo += "CPU Skinning Enabled";
        } else if ( currentModel->GetSkinningType() == GPU_SKINNING ) {
            modelInfo += "GPU Skinning Enabled";
        }

		currentModelText->SetText( consolasFont, modelInfo );
		currentModelText->SetPosition( glm::vec2( screenWidth  - 10.0f - currentModelText->GetWidth(),
												  screenHeight - 5.0f - ( currentModelText->GetHeight() / 2.0f ) ) );
	}
}