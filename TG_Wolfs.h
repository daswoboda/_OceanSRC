#pragma once
#ifndef T_WOLFS_H
#define T_WOLFS_H


#include <node3d.hpp>

namespace godot {

    class TG_Wolfs : public Node3D {
        GDCLASS(TG_Wolfs, Node3D)

    protected:
        static void _bind_methods();

    public:
        TG_Wolfs();
        ~TG_Wolfs();

        void _process(double delta) override;

        void set_WolfsNumber(const int velocity);

        int get_WolfsNumber() const;

        void _ready() override;



        //EXPORT
    private:
        int wolfsNumber{ 1 };






    };

}

#endif

