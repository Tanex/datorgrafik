
/** Loads the sourcecode from the given file into the given
    (previously created) shader object. Performs a compilation and
    writes resulting info log to the corresponding logfile and if
    there is compilation errors also to the console. 
    Returns zero on success.
*/
int compileShader(GLuint shader, const char *shaderSourceName, const char *logOutputName);

/** Links the previously created (and attached) program. Saves the
    infoLog in the given log file in case of failures. 
    Returns zero on success. */
int linkProgram(GLuint program, const char *logfile);

/** Loads the 2D image in the given file into the previously created
    (2D) texture object, using SDL_image for loading of the image
    file.  
    Returns zero on success.
*/
int loadTexture(GLuint texture, const char *filename, int generateMipmaps);
