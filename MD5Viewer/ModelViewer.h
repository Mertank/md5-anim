#ifndef GAME_H_
#define GAME_H_

#include "GLSH.h"
#include "Program.h"
#include "MD5Model.h"

/*
=============================

    ModelViewer

        The ModelViewer Application.
        Extends glsh::App.

=============================
*/
class ModelViewer : public glsh::App {
public:
							ModelViewer( void );
							~ModelViewer( void );

	void                    initialize( int w, int h )  override;
	void                    shutdown( void )            override;
	void                    resize( int w, int h )      override;
	void                    draw( void )                override;
	bool                    update( float dt )			override;

private:
    void                    LoadModels( void );
	void					UpdateCurrentModelInfo( void );
	
	bool					animateModel;
    int                     modelIndex;

	float					screenWidth;
	float					screenHeight;
	
	glsh::FreeLookCamera*	mainCamera;	
	
    Program*				CPUSkinningProgram;
    Program*				GPUSkinningProgram;
    Program*				textProgram;
	
    glm::mat4				projectionMatrix;
    glm::mat4				orthoMatrix;

	std::vector<MD5Model*>	models;
	MD5Model*				currentModel;

    glsh::TextBatch*        currentModelText;
    glsh::Font*             consolasFont;
};

#endif
