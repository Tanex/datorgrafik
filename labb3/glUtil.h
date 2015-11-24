
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

class AC3DMaterial {
 public:
  char name[32];
  GLfloat rgb[3];
  GLfloat amb[3];
  GLfloat emis[3];
  GLfloat spec[3];
  GLfloat shi, trans;
};
class AC3DSurface {
 public:
  int material;
  int flags;
  int nVertices;
  class Vertice {
  public:
    int index;
    GLfloat uv[2];
  };
  Vertice vert[4];  
};
class AC3DObject {
 public:
  AC3DObject();
  virtual void finalize();

  glm::mat4 mat;
  char type[32], name[128], textureName[128], url[128];
  GLfloat textureRepeat[2];
  int nChildren;
  class AC3DObject **children;
 
  int nVertices, nSurfaces, nQuads, nTriangles;
  /** Pointer to buffer of nVertices*4 GLfloat's */
  GLfloat *vertices;
  AC3DSurface *surfaces;
};

class AC3DObjectFactory {
 public:  
  AC3DObjectFactory();
  ~AC3DObjectFactory();

  /** Constructs the next object to be populated with data. If you
      want to implement a new form of objects to be parsed and created
      from AC3D files you should inherit from the AC3DObject class and
      from the AC3DObjectFactory -- letting this function create
      instances of your new object class. */
  virtual AC3DObject *makeObject();

  /** Attempts to open and parse the given ac3d file. Returns a
      reference to the first object in the file if
      successfull. 
      Returns NULL on any error. */
  AC3DObject *loadAC3D(const char *filename);  
 private:
  /** Parses a new object from file and populates a correpondigly created object (using self as factory). */
  AC3DObject *parseObject(FILE *fp);

  /** Temporary list of materials while parsing an AC3D file. */
  int nMaterials,  allocatedMaterials;
  AC3DMaterial *materials; 

  int currentDepth;
};
