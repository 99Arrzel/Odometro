import { useState } from 'react'

import './App.css'

function App() {
  /* Websocket  */

  const [count, setCount] = useState(0)
  const [ipconectar, setIpconectar] = useState("192.168.137.156:81");
  const [ws, setWs] = useState(null);

  const conectar = () => {
    const wsc = new WebSocket(`ws://${ipconectar}`);
    console.log("Conectando", ipconectar)
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
    }

    setWs(wsc);
  };
  //Envia por websocket set:0
  const setearCero = () => {
    ws.send("set:0");
  };


  return (
    <>
      <div>

        <h1>Contador</h1>
        <h2>IP: {ipconectar}</h2>
        <input type="text" value={ipconectar} onChange={(e) => setIpconectar(e.target.value)} />
        <button onClick={() => {
          conectar()
        }}>Conectar</button>
        <button onClick={() => {
          setearCero()
        }}>Setear 0</button>

        <h2>{count}</h2>
      </div>
    </>
  )
}

export default App
