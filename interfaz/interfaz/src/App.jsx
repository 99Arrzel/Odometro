import { useState } from "react";

import "./App.css";

function App() {
  /* Websocket  */

  const [count, setCount] = useState(0);
  const [ipconectar, setIpconectar] = useState("192.168.137.156:81");
  const [ws, setWs] = useState(null);

  const conectar = () => {
    const wsc = new WebSocket(`ws://${ipconectar}`);
    console.log("Conectando", ipconectar);
    wsc.onopen = () => {
      console.log("Conectado 💣💣💣💣");
    };
    wsc.onmessage = (e) => {
      console.log(e.data, "recibido!");
      setCount(e.data);
    };
    wsc.onclose = () => {
      console.log("Desconectado");
    };
    wsc.onerror = (err) => {
      console.log(err);
    };

    setWs(wsc);
  };
  //Envia por websocket set:0
  const setearCero = () => {
    ws.send("set:0");
  };
  const [mediciones, setMediciones] = useState([]);
  const [multiplicador, setMultiplicador] = useState(0.1754385964912281);
  return (
    <>
      <div className="flex gap-2 bg-gray-700 text-white h-screen">
        <div>
          <h1 className="text-2xl">Contador</h1>
          <h2>IP: {ipconectar}</h2>
          <input
            type="text"
            className="bg-gray-500 p-2 rounded-lg m-2"
            value={ipconectar}
            onChange={(e) => setIpconectar(e.target.value)}
          />
          <button
            onClick={() => {
              conectar();
            }}
            className="bg-green-500 p-2 rounded-lg m-2"
          >
            Conectar
          </button>
          <button
            onClick={() => {
              setearCero();
            }}
            className="bg-green-500 p-2 rounded-lg"
          >
            Setear 0
          </button>
          <h2>{`${count} / ${(count * multiplicador).toFixed(2)}`}</h2>
          <div className="flex">
            <button
              onClick={() => {
                //setear la tabla con medicion y la nueva fecha
                const medicionNueva = {
                  valor: count,
                  fecha: new Date(),
                };
                setMediciones([...mediciones, medicionNueva]);
              }}
              className="bg-green-500 p-2 rounded-lg m-2"
            >
              Guardar cambio
            </button>
          </div>
          <div className="flex">
            <p>Multiplicador</p>
            <input type="number" className="bg-gray-500 p-2 rounded-lg m-2" value={multiplicador} onChange={(e) => setMultiplicador(e.target.value)} />
          </div>
        </div>
        <div>
          <p className="text-2xl">Tabla de valores</p>
          <table className="table-auto">
            <thead>
              <tr>
                <th>Medicion</th>
                <th>Actual - anterior</th>
                <th>Pasos</th>
                <th>Fecha</th>
              </tr>
            </thead>
            <tbody>
              {mediciones.map((medicion, index) => (
                <tr key={index}>
                  <td>{Math.abs(medicion.valor * multiplicador).toFixed(2)}</td>
                  {
                    index > 0 ? (<td>{(Math.abs(medicion.valor * multiplicador) - Math.abs(mediciones[index-1].valor * multiplicador)).toFixed(2)}</td>) : (<td>0</td>)
                  }
                  <td>{Math.abs(medicion.valor) }</td>
                  <td>{medicion.fecha.toLocaleString()}</td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      </div>
    </>
  );
}

export default App;
