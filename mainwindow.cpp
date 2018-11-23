#include "mainwindow.h"
#include "mywindow.h"
#include <iostream>
#include <fstream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
      opengl = new mywindow(this);

      action_project_new_ =
          new QAction(QIcon(":/media/project-new.png"), tr("导入文件"), this);
      action_project_new_->setShortcuts(QKeySequence::New);
      connect(action_project_new_, &QAction::triggered, this,
              &MainWindow::Import);

      action_grab_movie_ =
          new QAction(QIcon(":/media/project-open.png"), tr("抓取动画"), this);
      action_grab_movie_->setShortcuts(QKeySequence::Open);
      connect(action_grab_movie_, &QAction::triggered, opengl,
              &mywindow::GrabMovies);
      action_reset_ =
              new QAction(QIcon(":/media/project-open.png"), tr("显示开关"), this);
          action_reset_->setShortcuts(QKeySequence::Delete);
          connect(action_reset_, &QAction::triggered, opengl,
                  &mywindow::Clear);
      light_control_ =
                  new QAction(QIcon(":/media/project-open.png"), tr("灯光控制"), this);

              connect(light_control_, &QAction::triggered, opengl,
                      &mywindow::LightContro);


       file_menu=menuBar()->addMenu(tr("选项"));

       file_menu->addAction(action_project_new_);
       file_menu->addAction(action_grab_movie_);
       file_menu->addAction(action_reset_);
       file_menu->addAction(light_control_);



       setCentralWidget(QWidget::createWindowContainer(opengl, this,
                                                     Qt::MSWindowsOwnDC));
//    opengl->isVisible();
//    opengl->PaintGL();

}

MainWindow::~MainWindow()
{

}


bool IsNotWhiteSpace(const int character) {
  return character != ' ' && character != '\n' && character != '\r' &&
         character != '\t';
}

void MainWindow::Import()
{
    const std::string path =
          QFileDialog::getOpenFileName(this, tr("Select source..."), "")
              .toUtf8()
              .constData();

    QFileInfo file_info(QString::fromStdString(path));
    QString temp_name = file_info.absolutePath() + "/";
    qDebug()<<" path"<<temp_name;




      if (path == "")
      {
        QMessageBox::warning(NULL, "error", "no such file in dir ", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return;
      }
    std::ifstream file(path, std::ios::binary);
    std::string line;
    bool in_vertex_section = false;

    int X_index = -1;
    int Y_index = -1;
    int Z_index = -1;
    int NX_index = -1;
    int NY_index = -1;
    int NZ_index = -1;
    int T1_X_index = 5;
    int T1_Y_index = 6;
    int T2_X_index = 7;
    int T2_Y_index = 8;
    int T3_X_index = 9;
    int T3_Y_index = 10;
    int R_index = -1;
    int G_index = -1;
    int B_index = -1;
    int X_byte_pos = -1;
    int Y_byte_pos = -1;
    int Z_byte_pos = -1;
    int R_byte_pos = -1;
    int G_byte_pos = -1;
    int B_byte_pos = -1;
    int tex_index = 11;
    size_t num_bytes_per_line = 0;
    size_t num_vertices = 0;
    int index = 0;



    QTime time;
    time.start();
  while (std::getline(file, line))
  {
    StringTrim(&line);

    if (line.empty()) {
      continue;
    }

    if (line == "end_header") {
      break;
    }
     const std::vector<std::string> line_elems = StringSplit(line, " ");

     if(line_elems[0]== "comment"&&line_elems[1]== "TextureFile")
     {
         QString tex = temp_name.right(temp_name.length()).append(QString::fromStdString(line_elems[2]));


         opengl->texture_names.push_back(tex);
     }
     if (line_elems.size() >= 3 && line_elems[0] == "element")
     {
          in_vertex_section = false;
          if (line_elems[1] == "vertex") {
            num_vertices = std::stoll(line_elems[2]);
            in_vertex_section = true;
          } else if (line_elems[1] == "face") {

          }
     }
     if (!in_vertex_section) {
          continue;
        }
     if (line_elems.size() >= 3 && line_elems[0] == "property")
     {

       if (line == "property float x") {
         X_index = index;
         X_byte_pos = num_bytes_per_line;
       } else if (line == "property float y") {
         Y_index = index;
         Y_byte_pos = num_bytes_per_line;
       } else if (line == "property float z") {
         Z_index = index;
         Z_byte_pos = num_bytes_per_line;
       } else if (line == "property float nx") {
           NX_index = index;
           //Y_byte_pos = num_bytes_per_line;
         } else if (line == "property float ny") {
           NY_index = index;
           //Z_byte_pos = num_bytes_per_line;
         } else if (line == "property float nz") {
           NZ_index = index;
           //Z_byte_pos = num_bytes_per_line;
         } else if (line == "property uchar r" || line == "property uchar red" ||
                  line == "property uchar diffuse_red" ||
                  line == "property uchar ambient_red" ||
                  line == "property uchar specular_red") {
         R_index = index;
         R_byte_pos = num_bytes_per_line;
       } else if (line == "property uchar g" || line == "property uchar green" ||
                  line == "property uchar diffuse_green" ||
                  line == "property uchar ambient_green" ||
                  line == "property uchar specular_green") {
         G_index = index;
         G_byte_pos = num_bytes_per_line;
       } else if (line == "property uchar b" || line == "property uchar blue" ||
                  line == "property uchar diffuse_blue" ||
                  line == "property uchar ambient_blue" ||
                  line == "property uchar specular_blue") {
         B_index = index;
         B_byte_pos = num_bytes_per_line;
       }

       index += 1;
       if (line_elems[1] == "float") {
         num_bytes_per_line += 4;
       } else if (line_elems[1] == "uchar") {
         num_bytes_per_line += 1;
       } else {

       }
     }



  }
   const bool is_rgb_missing = R_index == -1 || G_index == -1 || B_index == -1;

   qDebug()<<" size "<<opengl->texture_names.size();
   opengl->num_texures = new size_t[opengl->texture_names.size()]();

   std::vector<Eigen::Vector6f> Points;
   std::vector<Eigen::Vector3i> vertices;
   std::vector<Eigen::Vector6f> textures;

   while (std::getline(file, line))
   {
       const std::vector<std::string> line_elems = StringSplit(line, " ");
       if (line_elems.size() > 8)
       {

           opengl->num_texures[std::stoi(line_elems.at(tex_index))]++;


            Eigen::Vector3i vertice;

            vertice(0) = std::stoi(line_elems.at(1));
            vertice(1) = std::stoi(line_elems.at(2));
            vertice(2) = std::stoi(line_elems.at(3));

            Eigen::Vector6f texture;


            texture(0) = std::stof(line_elems.at(T1_X_index));
            texture(1) = std::stof(line_elems.at(T1_Y_index));
            texture(2) = std::stof(line_elems.at(T2_X_index));
            texture(3) = std::stof(line_elems.at(T2_Y_index));
            texture(4) = std::stof(line_elems.at(T3_X_index));
            texture(5) = std::stof(line_elems.at(T3_Y_index));




            vertices.push_back(vertice);
            textures.push_back(texture);

           break;
       }
        StringTrim(&line);
        std::stringstream line_stream(line);

        std::string item;
        std::vector<std::string> items;
        while (!line_stream.eof())
        {
          std::getline(line_stream, item, ' ');
          StringTrim(&item);
          items.push_back(item);
        }


        Eigen::Vector6f xyz;
        xyz(0) = std::stof(items.at(X_index));
        xyz(1) = std::stof(items.at(Y_index));
        xyz(2) = std::stof(items.at(Z_index));

        xyz(3) = std::stof(items.at(NX_index));
        xyz(4) = std::stof(items.at(NY_index));
        xyz(5) = std::stof(items.at(NZ_index));



        Points.push_back(xyz);


        Eigen::Vector3i rgb;
        if (is_rgb_missing) {
          rgb.setZero();
        } else {
          rgb(0) = std::stoi(items.at(R_index));
          rgb(1) = std::stoi(items.at(G_index));
          rgb(2) = std::stoi(items.at(B_index));
        }


     }
   opengl->_Points = Points;


   while (std::getline(file, line))
   {
       StringTrim(&line);
       std::stringstream line_stream(line);


       std::string item;
       std::vector<std::string> items;
       while (!line_stream.eof())
       {
         std::getline(line_stream, item, ' ');
         StringTrim(&item);
         items.push_back(item);

       }

        opengl->num_texures[std::stoi(items.at(tex_index))]++;
        Eigen::Vector3i vertice;
        vertice(0) = std::stoi(items.at(1));
        vertice(1) = std::stoi(items.at(2));
        vertice(2) = std::stoi(items.at(3));

        Eigen::Vector6f texture;

        texture(0) = std::stof(items.at(T1_X_index));
        texture(1) = std::stof(items.at(T1_Y_index));
        texture(2) = std::stof(items.at(T2_X_index));
        texture(3) = std::stof(items.at(T2_Y_index));
        texture(4) = std::stof(items.at(T3_X_index));
        texture(5) = std::stof(items.at(T3_Y_index));


        vertices.push_back(vertice);
        textures.push_back(texture);

   }
qDebug()<<time.elapsed()/1000.0<<"s";
qDebug()<<" duo shao ge "<<opengl->num_texures[11];


   opengl->_Vertices = vertices;
   opengl->_Textures = textures;
   opengl->initTextures();
   opengl->Upload();



}

void MainWindow::GrabMovies()
{


    //opengl->movie_grabber_widget_->show();
}

void MainWindow::Reset()
{

}

void MainWindow::StringTrim(std::string *str)
{
    StringLeftTrim(str);
    StringRightTrim(str);
}

void MainWindow::StringLeftTrim(std::string *str)
{
    str->erase(str->begin(),
                 std::find_if(str->begin(), str->end(), IsNotWhiteSpace));
}

void MainWindow::StringRightTrim(std::string *str)
{
    str->erase(std::find_if(str->rbegin(), str->rend(), IsNotWhiteSpace).base(),
               str->end());
}

std::vector<std::string> MainWindow::StringSplit(const std::string &str, const std::string &delim)
{
    std::vector<std::string> elems;
     boost::split(elems, str, boost::is_any_of(delim), boost::token_compress_on);
     return elems;
}
