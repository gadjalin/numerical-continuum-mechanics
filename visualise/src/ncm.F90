module ncm
    use, intrinsic :: iso_fortran_env

    implicit none
    private
    save

    type, public :: vertex_t
        real(real32) :: x, y
    end type vertex_t

    type, public :: edge_t
        integer, dimension(2) :: node_id
    end type edge_t

    type, public :: element_t
        integer, dimension(3) :: node_id
    end type element_t

    type, public :: mesh_t
        type(vertex_t), dimension(:), allocatable :: nodes
        type(edge_t), dimension(:), allocatable :: edges
        type(element_t), dimension(:), allocatable :: elements
    end type mesh_t

    public :: read_mesh

    contains

        function read_mesh(filename) result(mesh)
            implicit none
            character(*), intent(in) :: filename
            type(mesh_t)               :: mesh

            integer :: mesh_unit
            integer :: iostat

            integer :: vertex_count, edge_count, element_count
            integer :: i

            open(newunit=mesh_unit, file=filename, status='old', action='read', iostat=iostat)
            if (iostat /= 0) stop 'Unable to read mesh file ' // filename

            read (mesh_unit, '(I8)') vertex_count
            if (vertex_count > 0) then
                allocate(mesh%nodes(vertex_count))
                read (mesh_unit, '(2F12.5)') mesh%nodes
            end if

            read (mesh_unit, '(I8)') edge_count
            if (edge_count > 0) then
                allocate(mesh%edges(edge_count))
                read (mesh_unit, '(2I8)') mesh%edges
            end if

            read (mesh_unit, '(I8)') element_count
            if (element_count > 0) then
                allocate(mesh%elements(element_count))
                read (mesh_unit, '(3I8)') mesh%elements
            end if

            close(mesh_unit)
        end function read_mesh
end module ncm
