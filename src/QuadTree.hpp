#ifndef SOURCE_QUAD_TREE_HPP
#define SOURCE_QUAD_TREE_HPP

#include <array>
#include <iostream>
#include <fstream>
#include <math.h>
#include <memory>
#include <string>
#include <vector>

namespace ads {
    namespace Geo2d {
        template<typename Numeric_type>
        struct Point2D{
            Point2D(){
                x_ = 0;
                y_ = 0;
                quadrant = 0;
            }

            Point2D(const Numeric_type x, const Numeric_type y){
                x_ = x;
                y_ = y;
                set_quadrant();
            }

            void operator()(const Numeric_type x, const Numeric_type y){
                x_ = x;
                y_ = y;
                set_quadrant();
            }

            Numeric_type get_x() const noexcept {
                return x_;
            }

            Numeric_type get_y() const noexcept {
                return y_;
            }

            std::size_t get_quadrant() const noexcept {
                return quadrant;
            }

        private:
            Numeric_type x_, y_;
            std::size_t quadrant;

            void set_quadrant() {
                quadrant = ((x_ / abs(x_)) < 0) + (((y_ / abs(y_)) < 0)<<1);
            }
        };

        class QuadTree {
        protected:
            std::array<std::shared_ptr<QuadTree>, 4> sub_quads;

            virtual void load_data_from_file(std::string path) = NULL;
        };

        class QuadTreeImgOpt : public QuadTree {
            typedef std::pair<Point2D<uint32_t>, uint32_t> Pixel;
        public:
            QuadTreeImgOpt() {

            }

            virtual ~QuadTreeImgOpt() { sub_quads.fill(nullptr); }

            void load_data_from_file(std::string path) {

            }
        };

        template<typename Numeric_type>
        class QuadTreeRegion : public QuadTree {
            typedef Point2D<Numeric_type> SpatialObj;

            bool was_splitted;
            std::size_t fill_factor_;
            SpatialObj key;
            std::vector<std::shared_ptr<SpatialObj>> content;

            bool should_be_leaf() {
                return content.size() >= fill_factor_;
            }

            void calculate_key(const SpatialObj& point, bool operation) {
                key(key.get_x() + point.get_x() / (content.size()+((!operation)?1:-1)),
                    key.get_y() + point.get_y() / (content.size()+((!operation)?1:-1)));
            }

            void go_down(const SpatialObj& new_point) {
                std::size_t cquad = new_point.get_quadrant();
                std::shared_ptr<QuadTreeRegion> next_node =
                                   static_cast<QuadTreeRegion>(sub_quads[cquad]);
                next_node->append(new_point);
            }

        public:
            QuadTreeRegion(const std::size_t fill_factor = 1) :
                                fill_factor_(fill_factor),
                                was_splitted(false) { key(0, 0);}

            virtual ~QuadTreeRegion() { sub_quads.fill(nullptr); }

            void load_data_from_file(std::string path) {

            }

            void append(const SpatialObj& new_point) {
                if (was_splitted){//internal node
                    go_down(new_point);
                }
                else if (should_be_leaf()) {//upgrade to leaf
                    for(std::shared_ptr<QuadTreeRegion>& node : sub_quads){
                        node = std::make_shared<QuadTreeRegion>(fill_factor);
                    }
                    was_splitted = true;
                    go_down(new_point);
                }
                else{//already leaf
                    content.push_back(std::make_shared<SpatialObj>(newpoint));
                    calculate_key(new_point, true);
                }
            }
        };
    }
}

#endif //SOURCE_QUAD_TREE_HPP