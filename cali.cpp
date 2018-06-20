/* -*- tab-width: 4 -*- */
//cygwin so far - g++ -g -I/usr/local/include/mapnik -I/opt/local/include -I/opt/local/include/freetype2 -I../../agg/include -L/usr/local/lib -L/opt/local/lib -Wl,--start-group -lmapnik  -lboost_thread -licuuc -Wl,--end-group cali.cpp -o cali 2>&1 | less
//-DBIGINT affects value_integer in mapbox.hpp, needs to match how library was built.
//cygwin - g++ -m64 -DBIGINT -g -I/usr/local/include/mapnik -I/opt/local/include -I/opt/local/include/freetype2 -I../../agg/include -L/usr/local/lib -L/opt/local/lib -L/cygdrive/f/dev/mapnik/cygwin64/mapnik.v3.x.xx.D.git/src  cali.cpp -o cali.exe -Wl,-dll-search-prefix=cyg -Wl,--start-group -lmapnik -lboost_thread -licuuc -Wl,--end-group
//cygwin - g++ -m64 -DBIGINT -g -I/usr/local/include/mapnik -I/opt/local/include -I/opt/local/include/freetype2 -I../../agg/include -L/usr/local/lib -L/opt/local/lib -L/cygdrive/f/dev/mapnik/cygwin64/mapnik.v3.x.xx.D.git/src  cali.cpp -o cali.exe -Wl,-dll-search-prefix=cyg -Wl,--start-group -lmapnik -lboost_thread -licuuc -Wl,--end-group

/*****************************************************************************
 * Example code modified from Mapnik rundemo.cpp (r727, license LGPL). It
 * (1) Renders the State of California using USGS state boundaries data.
 * (2) Plots a set of California fourteeners as point symbols
 *****************************************************************************/

// define before any includes
#define BOOST_SPIRIT_THREADSAFE

#include <mapnik/map.hpp>
#include <mapnik/layer.hpp>
#include <mapnik/rule.hpp>
#include <mapnik/feature_type_style.hpp>
#include <mapnik/symbolizer.hpp>
#include <mapnik/text/placements/dummy.hpp>
#include <mapnik/text/text_properties.hpp>
#include <mapnik/text/formatting/text.hpp>
#include <mapnik/datasource_cache.hpp>
#include <mapnik/font_engine_freetype.hpp>
#include <mapnik/agg_renderer.hpp>
#include <mapnik/expression.hpp>
#include <mapnik/color_factory.hpp>
#include <mapnik/image_util.hpp>
#include <mapnik/unicode.hpp>
#include <mapnik/save_map.hpp>
#include <mapnik/cairo_io.hpp>
#include <mapnik/config_error.hpp>
#include <mapnik/feature_factory.hpp>
#include <mapnik/memory_datasource.hpp>
#include <mapnik/load_map.hpp>
#include <mapnik/geometry.hpp>
#include <mapnik/vertex.hpp>
#include <mapnik/feature_type_style.hpp>
#include <mapnik/json/json_grammar_config.hpp>
#include <mapnik/symbolizer_keys.hpp>
#include <mapnik/css_color_grammar_x3_def.hpp>
#include <mapnik/image_filter_types.hpp>
#include <mapnik/geometry/point.hpp>

#include <iostream>

// This class implements a simple way of displaying point-based data
//
class point_datasource : public mapnik::memory_datasource {
public:
	point_datasource(mapnik::parameters const &params) : mapnik::memory_datasource(params), feat_id(0) {}
	void add_point(double x, double y, const char* key, const char* value);
    decltype(mapnik::datasource::datasource_t::Raster) type() const { return mapnik::datasource::datasource_t::Raster; }

private:
	int feat_id;
};

using oldvertex2d = mapnik::vertex2d;
using geometry2d = mapnik::geometry::geometry<oldvertex2d>;
void point_datasource::add_point(double x, double y, 
				 const char* key, const char* value)
{
	using namespace mapnik;
    auto ctx = std::make_shared<mapnik::context_type>();
	feature_ptr feature(feature_factory::create(ctx,feat_id++));
    mapnik::geometry::point<double> apoint(x,y);
    feature->set_geometry(apoint);
	transcoder tr("utf-8");
    feature->put(key,tr.transcode(value));
	this->push(feature);
}

using Color = mapnik::color;

int main ( int argc , char** argv)
{
    if (argc != 2)
    {
        std::cout << "usage: $0 <mapnik_install_dir>\n";
        return EXIT_SUCCESS;
    }

    using namespace mapnik;
    try {
        std::string mapnik_dir(argv[1]);
        datasource_cache::instance().register_datasources(mapnik_dir + "/plugins/input/shape");
        freetype_engine::register_font(mapnik_dir + "/fonts/dejavu-ttf-2.37/ttf/DejaVuSans.ttf");

        Map m(1080,680);
        m.set_background(Color(220, 226, 240));
        //from newer demo... 
        //m.set_srs(srs_merc);

		// Load Mountain-style from XML
		bool strict = true;
		mapnik::load_map(m, "style.xml", strict);

        // create styles

        // States (polygon)
        mapnik::feature_type_style other_style;

        // non-CA states (polyline)
        mapnik::feature_type_style provlines_style;

        //mapnik::keys::stroke provlines_stk (Color(127,127,127),0.75);
        //provlines_stk.add_dash(10, 6);
        //mapnik::json::rule_type provlines_rule;
        {
            rule r;
            {
                line_symbolizer line_sym;
                //put(line_sym,keys::stroke,color(0,0,0));
                //put(line_sym,keys::stroke_width,1.0);
                put(line_sym,keys::stroke,color(127,127,127));
                put(line_sym,keys::stroke_width,0.75);
                dash_array dash;
                //dash.emplace_back(8,4);
                //dash.emplace_back(2,2);
                //dash.emplace_back(2,2);
                dash.emplace_back(10, 6);
                put(line_sym,keys::stroke_dasharray,dash);
                r.append(std::move(line_sym));
            }
            provlines_style.add_rule(std::move(r));

            //rule provlines_rule(r);
            rule provlines_rule(std::move(r));
            //provlines_rule.append(mapnik::polygon_symbolizer(mapnik::color("cornsilk")));
            //provlines_rule.append(mapnik::polygon_symbolizer(mapnik::css_color_grammar::named_colors::from_string("cornsilk")));
            //provlines_rule.append(line_symbolizer(provlines_stk));
            //provlines_rule.append(std::move(r));
            //provlines_rule.set_filter(mapnik::create_filter("[STATE] <> 'California'"));
            provlines_rule.set_filter(parse_expression("[STATE] <> 'California'"));
            {
                //provlines_rule.append(mapnik::polygon_symbolizer(mapnik::css_color_grammar::named_colors::from_string("cornsilk")));
                polygon_symbolizer poly_sym;
                //put(poly_sym, keys::fill, mapnik::css_color_grammar::named_colors::from_string("cornsilk"));
                put(poly_sym, keys::fill, mapnik::color("cornsilk"));
                provlines_rule.append(std::move(poly_sym));
                
            }
            //other_style.add_rule(provlines_rule);
            other_style.add_rule(std::move(provlines_rule));
        }
        m.insert_style("provlines", std::move(provlines_style));
        m.insert_style("elsewhere", other_style);

        // Layers
        // Provincial  polygons
        {
            parameters p;
            p["type"]="shape";
            p["file"]="../usgs_statesp020/statesp020"; // State Boundaries of the United States [SHP]
            //p["encoding"] = "utf8";

            layer lyr("Cali");
            lyr.set_datasource(datasource_cache::instance().create(p));
            lyr.add_style("cali"); // style.xml
            lyr.add_style("elsewhere"); // this file
            m.add_layer(lyr);
        }

        #if 1
        //TBD: figure out how to update from this to current mapnik APIs...
		// Mountain data points
        {
            mapnik::parameters params;
            params["type"] = "memory";
			point_datasource* pds = new point_datasource(params);
			pds->add_point(-118.29, 36.58, "name", "mount Whitney");
			pds->add_point(-118.31, 36.65, "name", "mount Williamson");
			pds->add_point(-118.25, 37.63, "name", "White mountain");
			pds->add_point(-122.19, 41.41, "name", "mount Shasta");
			pds->add_point(-118.24, 37.52, "name", "mount Langley");
			// ...
			datasource_ptr ppoints_data(pds);

			// Plot datapoints
			mapnik::layer lyr("Mountains");
			lyr.set_datasource(ppoints_data);
			lyr.add_style("mtn");
			m.add_layer(lyr);
        }
        #endif

        // Get layer's featureset
        mapnik::layer lay = m.get_layer(0);
        mapnik::datasource_ptr ds = lay.datasource();
        //mapnik::query q(lay.envelope(), 1.0); // what does this second param 'res' do?
        //mapnik::query q(ds->envelope(), 1.0); // what does this second param 'res' do?
        mapnik::query q(ds->envelope()); // (maybe second param 'res' did nothing)
        q.add_property_name("STATE"); // NOTE: Without this call, no properties will be found!
        mapnik::featureset_ptr fs = ds->features(q);
        // One day, use filter_featureset<> instead?

        //Envelope<double> extent; // NOTE: nil extent = <[0,0], [-1,-1]>
        box2d<double> extent; //TBD: still same defaults?

        // Loop through features in the set, get extent of those that match query && filter
        feature_ptr feat = fs->next();
        //filter_ptr cali_f(create_filter("[STATE] = 'California'"));
        using filter_ptr = boost::shared_ptr<filter::filter_type>;
        //filter_ptr cali_f(parse_expression("[STATE] = 'California'"));
        //decltype(parse_expression("[STATE] = 'California'")) cali_f(parse_expression("[STATE] = 'California'"));
        //std::cerr << cali_f->to_string() << std::endl; // NOTE: prints (STATE=TODO)!
        //std::cerr << cali_f << std::endl; // NOTE: prints (STATE=TODO)!
        while (feat) {
            //implicity in 'next()' now???... if (cali_f->pass(*feat)) 
            {
                for  (unsigned i=0; i<feat->size();++i) {
                    //geometry2d & geom = feat->get_geometry(i);
                    //TBD: defaults as commented above, or changes needed for extent().<dims> ???
                    if (extent.width() < 0 && extent.height() < 0) {
						// NOTE: expand_to_include() broken w/ nil extent. Work around.
                        //extent = geom.envelope();
                        extent = feat->envelope();
                    }
                    //extent.expand_to_include(geom.envelope());
                    extent.expand_to_include(feat->envelope());
                }
            }
            feat = fs->next();
		}

        m.zoom_to_box(extent);
        m.zoom(1.15); // zoom out slightly

	mapnik::image_rgba8 buf(m.width(),m.height());
        agg_renderer<mapnik::image_rgba8> ren(m,buf);
        ren.apply();

        //save_to_file<image_rgba8>(buf.data(),"cali.png","png");
        save_to_file<image_rgba8>(buf,"cali.png","png");
    }
    catch ( const mapnik::config_error & ex )
    {
            std::cerr << "### Configuration error: " << ex.what();
            return EXIT_FAILURE;
    }
    catch ( const std::exception & ex )
    {
            std::cerr << "### std::exception: " << ex.what();
            return EXIT_FAILURE;
    }
    catch ( ... )
    {
            std::cerr << "### Unknown exception." << std::endl;
            return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
