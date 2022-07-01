#include "tienda.h"
#include "ui_tienda.h"

Tienda::Tienda(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Tienda)
{
    ui->setupUi(this);
    // Lista de productos
    cargarProductos();
    // Mostrar los productos en el combo
    foreach (Producto *p, m_productos){
        ui->inProducto->addItem(p->nombre());
    }
    // Configurar cabecera de la tabla
    QStringList cabecera = {"Cantidad", "Producto", "P. unitario", "Subtotal"};
    ui->outDetalle->setColumnCount(4);
    ui->outDetalle->setHorizontalHeaderLabels(cabecera);
    // Establecer el subtotal a 0
    m_subtotal = 0;
    //    mModel = new QStandardItemModel(this);
    //    ui->outDetalle->setModel(mModel);
    //    setCentralWidget(ui->outDetalle);
    //    setWindowTitle("Du CSV Viewer");
}

Tienda::~Tienda()
{
    delete ui;
}
/**
 * @brief Tienda::cargarProductos Carga la lista de productos de la tienda
 */
void Tienda::cargarProductos()
{

    QDir actual = QDir::current(); //directorio actual
    // El path al archivo CSV
    QString archivoProductos = actual.absolutePath() + "/debug/productos.csv";
    QFile archivo(archivoProductos);
    //qDebug() << archivo.fileName();

    if (archivo.open(QIODevice::ReadOnly | QIODevice::Text)){

        bool primera = true;
        QTextStream in(&archivo);
        while (!in.atEnd()) {
            QString linea = in.readLine();
            if (primera){
                primera = false;
                continue;
            }
            QStringList datos = linea.split(";");
            QString precio = datos.at(2);
            float p = precio.toFloat();
            int id = datos.at(0).toInt();
            m_productos.append(new Producto(id, datos.at(1), p));
        }

        archivo.close();
    }else{
        qDebug()<< "No se pudo abrir el archivo";
    }



    //    Crear productos "quemados" en el código
    //    m_productos.append(new Producto(1, "Leche", 0.80));
    //    m_productos.append(new Producto(2, "Pan", 0.15));
    //    m_productos.append(new Producto(3, "Queso", 2.50));
    //    Podría leerse de una base de datos, de un archivo o incluso de Internet
}

void Tienda::calcular(float stProducto)
{
    // Calcular valores
    m_subtotal += stProducto;
    float iva = m_subtotal * IVA / 100;
    float total = m_subtotal + iva;
    // Mostrar valores en GUI
    ui->outSubtotal->setText("$ " + QString::number(m_subtotal, 'f', 2));
    ui->outIva->setText("$ " + QString::number(iva, 'f', 2));
    ui->outTotal->setText("$ " + QString::number(total, 'f', 2));
}

//void Tienda::setValueAt(int ix, int jx, const QString &value)
//{
//    if(!mModel->item(ix, jx)){
//        mModel->setItem(ix, jx, new QStandardItem(value));
//    }else{
//        mModel->item(ix, jx)->setText(value);
//    }
//}


void Tienda::on_inProducto_currentIndexChanged(int index)
{
    // Obtener el precio del producto actual seleccionado
    float precio = m_productos.at(index)->precio();
    // Mostrar el precio del product en la etiqueta
    ui->outPrecio->setText("$ " + QString::number(precio,'f',2));
    // Resetear el spinbox de cantidad
    ui->inCantidad->setValue(0);
}


void Tienda::on_btnAgregar_released()
{
    // Validar que no se agregen productos cpn 0 cantidad
    int cantidad = ui->inCantidad->value();
    if (cantidad == 0){
        return;
    }
    // Obtener los datos de la GUI
    int i = ui->inProducto->currentIndex();
    Producto *p = m_productos.at(i);

    // Calcular el subrotal del producto
    float subtotal = p->precio() * cantidad;

    // Agregar los datos a la tabla
    int fila = ui->outDetalle->rowCount();
    ui->outDetalle->insertRow(fila);
    ui->outDetalle->setItem(fila, 0, new QTableWidgetItem(QString::number(cantidad)));
    ui->outDetalle->setItem(fila, 1, new QTableWidgetItem(p->nombre()));
    ui->outDetalle->setItem(fila, 2, new QTableWidgetItem(QString::number(p->precio(),'f',2)));
    ui->outDetalle->setItem(fila, 3, new QTableWidgetItem(QString::number(subtotal,'f',2)));

    // Limpiar datos
    ui->inCantidad->setValue(0);
    ui->inProducto->setFocus();

    // Actualizar subtotales
    calcular(subtotal);

}

void Tienda::on_actionGuadar_triggered()
{
    // Abrir un cuadro de dialogo para seleccionar el path y archivo a guardadr
    QTextStream io;
    QString nombreArchivo = QFileDialog::getSaveFileName(this,"Guardar factura",QDir::current().absolutePath() + "/productos.csv","Archivos de calculo (*.csv)");
    QFile archivo;
    archivo.setFileName(nombreArchivo);
    archivo.open(QFile::WriteOnly | QFile::Truncate);
    if(!archivo.isOpen()){
        QMessageBox::critical(this,"Aviso","No se pudo abrir el archivo");
        return;
    }
    io.setDevice(&archivo);
    int fila = ui->outDetalle->rowCount();
    int columna = ui->outDetalle->columnCount();
    QString celda;
    for(int i=0; i<fila; i++){
        for(int j=0; j<columna; j++){
            if(j != (columna-1)){
                celda = ui->outDetalle->item(i,j)->text()+";";
            }else{
                celda = ui->outDetalle->item(i,j)->text();
            }

            io << celda;
        }

        io << "\n";
    }

    QMessageBox::information(this,"Aviso","Archivo guardado");
    archivo.flush();
    archivo.close();
}


//void Tienda::on_actionAbrir_triggered()
//{
//    auto nombreArchivo = QFileDialog::getOpenFileName(this,"Abrir",QDir::rootPath(), "Archivos csv (*.csv)");
//    if (nombreArchivo.isEmpty()){
//        return;
//    }

//    QFile archivo(nombreArchivo);
//    if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)){
//        return;
//    }

//    QTextStream salida(&archivo);
//    int filascCount = 0;
//    while(!salida.atEnd()){
//        mModel->setRowCount(filascCount);
//        auto line = salida.readLine();
//        auto valor = line.split(";");
//        const int columCount = valor.size();
//        mModel->setColumnCount(columCount);
//        for(int j = 0; j < columCount; j++){
//            setValueAt(filascCount, j, valor.at(j));
//        }
//        ++ filascCount;
//    }
//    archivo.close();
//}


void Tienda::on_actionNuevo_triggered()
{

    while(ui->outDetalle->rowCount() > 0){
        ui->outDetalle->removeRow(0);
    }
    ui->statusbar->showMessage("Nueva hoja de calculos",3000);

}

void Tienda::on_actionAcerca_de_triggered()
{
    acerca_de *dialog = new acerca_de(this);

    dialog->setVersion(VERSION);

    dialog->exec();
}


void Tienda::on_btnFacturar_clicked()
{
    if(ui->inCedula->displayText().isEmpty()){
        QMessageBox::warning(this, "Advertencia", "El campo de la cedula esta vacia");
        return;
    }else if(ui->inEmail->displayText().isEmpty()){
        QMessageBox::warning(this, "Advertencia", "El campo del E-mail esta vacio");
        return;
    }else if(ui->inNombre->displayText().isEmpty()){
        QMessageBox::warning(this, "Advertencia", "El campo del nombre esta vacio");
        return;
    }


}

