program visualise
    use glf90w
    use gl

    implicit none
    type(GLFWwindow) :: window

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

        call glBegin(GL_TRIANGLE_STRIP)
        call glColor3f(1.0, 1.0, 1.0)
        call glVertex2f(0.0, 0.0)
        call glColor3f(1.0, 1.0, 1.0)
        call glVertex2f(1.0, 0.0)
        call glColor3f(1.0, 1.0, 1.0)
        call glVertex2f(0.0, 1.0)
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

