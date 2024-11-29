program visualise
    use glf90w
    use gl
    use ncm

    implicit none
    type(GLFWwindow) :: window
    type(mesh_t) :: mesh

    integer :: i
    real :: viewport_size

    mesh = read_mesh('default.mesh')
    viewport_size = max(maxval(mesh%nodes(:)%x), maxval(mesh%nodes(:)%y))

    call glfwSetErrorCallback(error_callback)

    if (.not. glfwInit()) stop 'glfwInit'

    call glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3)
    call glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3)
    call glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE)

    window = glfwCreateWindow(800, 600, 'numerical continuum mechanics')
    if (.not. associated(window)) stop 'glfwCreateWindow'

    call glfwMakeContextCurrent(window)

    if (.not. gladLoadGL(glfwGetProcAddress)) stop 'gladLoadGL'

    do
        call glfwPollEvents()

        call glClearColor(0.28, 0.13, 0.34, 1.0)
        call glClear(GL_COLOR_BUFFER_BIT)

        call glPointSize(2.0)
        call glBegin(GL_POINTS)

        do i = 1,size(mesh%nodes)
            call glColor3f(1.0, 1.0, 1.0)
            call glVertex2f((mesh%nodes(i)%x/viewport_size)*0.95, (mesh%nodes(i)%y/viewport_size)*0.95)
        end do

        call glEnd()

        call glfwSwapBuffers(window)

        if (glfwWindowShouldClose(window)) exit
    end do

    call glfwDestroyWindow(window)
    call glfwTerminate()

    contains

        subroutine error_callback(code, description)
            implicit none
            integer, intent(in) :: code
            character(*), intent(in) :: description

            print '(''Error '',I8,'': '',A)', code, description
        end subroutine error_callback
end program visualise

