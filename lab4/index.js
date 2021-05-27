import { createInterface } from "readline";
const rl = createInterface({ input: process.stdin, output: process.stdout });

/** @returns {Promise<string>} */
const waitForInput = () => new Promise((res) => rl.question("", res));

/** @param {number} time */
const delay = (time) => new Promise((res) => setTimeout(res, time));

if (process.argv.length > 4 || process.argv.length < 3) {
  console.log("Size: number -> required");
  console.log("Simulation: true -> optional");
  process.exit(1);
}

const simulate = process.argv[3] === "true";
const n = parseInt(process.argv[2]);
const init = Array.from(new Array(n), (_, index) => (index + 1) % 10);

/** @type {(string | undefined)[]} */
let state = init.map(() => undefined);

const printState = () => {
  console.log(init.join(""));
  console.log(state.map((a) => (a === undefined ? "-" : a)).join(""));
  console.log("");
};

let id = 48;

/** @param {number} size */
const add = (size) => {
  let index = undefined;
  let count = 0;

  for (let i = 0; i < n; i++) {
    if (state[i] === undefined) {
      if (index === undefined) {
        index = i;
        count = 0;
      }

      count++;

      if (count === size) {
        if (i + 1 > n) {
          console.log("No space found");
          console.log(`Tried to add ${size} of [${String.fromCharCode(id)}]`);
          return;
        }

        console.log(`Adding ${size} of [${String.fromCharCode(id)}]`);

        for (let j = index; j < i + 1; j++) {
          state[j] = String.fromCharCode(id).toString();
        }

        id++;
        if (id === 58) id = 65;
        if (id === 90) id = 97;
        if (id === 122) id = 48;

        return;
      }
    } else {
      index = undefined;
    }
  }

  console.log("No space found");
  console.log(`Tried to add ${size} of [${String.fromCharCode(id)}]`);
};

/** @param {string} removeId */
const remove = (removeId) => {
  console.log(`Removing ${removeId}`);
  state = state.map((a) => (a === removeId ? undefined : a));
};

(async () => {
  if (simulate) {
    console.log("Running in auto mode");
  }

  while (1) {
    printState();

    if (simulate) {
      // AutoRun
      await delay(Math.floor(Math.random() * 2000) + 500);
      if (Math.random() > 0.3) {
        add(Math.floor(Math.random() * 9) + 1);
      } else {
        const items = [...new Set(state)].filter((a) => a !== undefined);
        if (items.length === 0) {
          console.log("Can't remove nothing in array");
          continue;
        }

        const item = items[Math.floor(Math.random() * items.length)];
        // @ts-ignore
        remove(item);
      }
    } else {
      const res = await waitForInput();

      if (res.startsWith("Z")) {
        add(parseInt(res.split(" ")[1]));
      } else if (res.startsWith("O")) {
        remove(res.split(" ")[1]);
      } else if (res.startsWith("Q")) {
        console.log("Bye");
        process.exit(0);
      } else {
        console.log("Wrong command skipping");
      }
    }
  }
})();
