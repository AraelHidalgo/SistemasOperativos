
\c aseguradora;
--Propietario
CREATE TABLE Propietario (
    id_prop SERIAL PRIMARY KEY,
    nombre VARCHAR(100) NOT NULL,
    ape_paterno VARCHAR(100),
    ape_materno VARCHAR(100),
    direccion TEXT,
    fecha_nacimiento DATE NOT NULL,
    edad INT,
    rfc VARCHAR(20) NOT NULL,
    --telefono VARCHAR(20),
    CHECK (edad > 17)
);

CREATE TABLE Telefono (
    propietario_id INT NOT NULL,
    tipo VARCHAR(20) CHECK (tipo IN ('Casa', 'Oficina', 'Celular')),
    numero VARCHAR(20) NOT NULL,
    PRIMARY KEY (propietario_id, tipo), --(tabla débil)
    FOREIGN KEY (propietario_id) REFERENCES Propietario(id_prop) ON DELETE CASCADE
);

--Funcion
CREATE OR REPLACE FUNCTION calcular_edad() 
RETURNS TRIGGER AS $$
BEGIN
    NEW.edad := EXTRACT(YEAR FROM AGE(CURRENT_DATE, NEW.fecha_nacimiento));

    -- Restricción de edad
    IF NEW.edad <= 17 THEN
        RAISE EXCEPTION 'La edad debe ser mayor de 17 años';
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

--Trigger creado
CREATE TRIGGER trigger_calcular_edad
BEFORE INSERT OR UPDATE ON Propietario
FOR EACH ROW
EXECUTE FUNCTION calcular_edad();


--Auto
CREATE TABLE Auto (
    id_auto SERIAL PRIMARY KEY,
    propietario_id INT,
    placas VARCHAR(20) UNIQUE NOT NULL,
    color VARCHAR(50) NOT NULL,
    modelo INT CHECK (modelo > 2010),
    tipo VARCHAR(50),
    FOREIGN KEY (propietario_id) REFERENCES Propietario(id_prop) ON DELETE CASCADE
);

-- Accidente
CREATE TABLE Accidente (
    auto_id INT NOT NULL,
    cns VARCHAR(20) NOT NULL,
    lugar VARCHAR(200) DEFAULT 'Tapachula',
    fecha DATE CHECK (fecha <= CURRENT_DATE),
    hora TIME,
    PRIMARY KEY (auto_id, cns),
    FOREIGN KEY (auto_id) REFERENCES Auto(id_auto) ON DELETE CASCADE
);


-- Insertar datos en la tabla Propietario
INSERT INTO Propietario (nombre, ape_paterno, ape_materno, direccion, fecha_nacimiento, edad, rfc)
VALUES ('Juan', 'Pérez', 'Gómez', 'Calle Falsa 123', '1980-05-15', 42, 'JPG800515HDFRZN01');

-- Insertar datos en la tabla Telefono
INSERT INTO Telefono (propietario_id, tipo, numero)
VALUES (1, 'Casa', '555-1234');

INSERT INTO Telefono (propietario_id, tipo, numero)
VALUES (1, 'Celular', '555-5678');

-- Insertar datos en la tabla Auto
INSERT INTO Auto (propietario_id, placas, color, modelo, tipo)
VALUES (1, 'ABC123', 'Rojo', 2015, 'Sedán');

-- Insertar datos en la tabla Accidente
INSERT INTO Accidente (auto_id, cns, lugar, fecha, hora)
VALUES (1, 'CNS123', 'Avenida Central', '2023-10-05', '14:30:00');